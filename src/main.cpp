#include "mainwindow.h"
#include "radar.h"
#include "types.h"
#include "constants.h"
#ifdef __linux__
    #include "sigwatch.h"
#endif

#include <QApplication>
#include <QThread>
#include <QDebug>
#include <QTime>

void messageHandler(QtMsgType type, QMessageLogContext const &, QString const & msg)
{
    switch (type)
    {
        case QtDebugMsg:
            fprintf(stdout, "%s [DEBUG]: %s\n", QTime::currentTime().toString().toStdString().c_str(), msg.toStdString().c_str());
            fflush(stdout);
            break;
        case QtInfoMsg:
            fprintf(stdout, "%s [INFO]: %s\n", QTime::currentTime().toString().toStdString().c_str(), msg.toStdString().c_str());
            fflush(stdout);
            break;
        case QtWarningMsg:
            fprintf(stderr, "%s [WARNING]: %s\n", QTime::currentTime().toString().toStdString().c_str(), msg.toStdString().c_str());
            break;
        case QtCriticalMsg:
            fprintf(stderr, "%s [CRITICAL]: %s\n", QTime::currentTime().toString().toStdString().c_str(), msg.toStdString().c_str());
            break;
        case QtFatalMsg:
            fprintf(stderr, "%s [FATAL]: %s\n", QTime::currentTime().toString().toStdString().c_str(), msg.toStdString().c_str());
            break;
    }
}

bool tryConnect(Radar & r)
{
    auto attempts = 0;
    while (attempts <  STARTUP_CONNECTION_ATTEMPS)
    {
        attempts++;
        qInfo() << "Connection attempt" << "(" << attempts << "/" << STARTUP_CONNECTION_ATTEMPS << ")";
        if (r.connect())
            break;
        QThread::msleep(STARTUP_CONNECTION_PAUSE_TIME);
    }

    if (attempts == STARTUP_CONNECTION_ATTEMPS)
    {
        qWarning() << "Aborted: Failed to connect to radar sensor. ";
        return false;
    }
    return true;
}

bool tryAddingEndpoints(Radar & r)
{
    qInfo() << "Trying to add base endpoint...";
    if (!r.addEndpoint(EndpointType_t::Base))
    {
        qCritical() << "Error: Failed to add base endpoint. ";
        return false;
    }
    qInfo() << "Successfully added base endpoint.";

    qInfo() << "Trying to add target detection endpoint...";
    if (!r.addEndpoint(EndpointType_t::TargetDetection))
    {
        qCritical() << "Error: Failed to add target detection endpoint. ";
        return false;
    }
    qInfo() << "Successfully added target detection endpoint.";

    return true;
}

bool trySettingUpFrameTrigger(Radar & r)
{
    qInfo() << "Trying to diable automatic frame trigger for base endpoint...";
    if (!r.setAutomaticFrameTrigger(false, EndpointType_t::Base, 0))
    {
        qCritical() << "Error: Failed to disable frame trigger for base endpoint. ";
        return false;
    }
    qInfo() << "Successfully disabled frame trigger for base endpoint.";

    return true;
}


int main(int argc, char *argv[])
{
    // Register the message handler within the application
    qInstallMessageHandler(messageHandler);
    QApplication a(argc, argv);

    // Signal watch for linux to catch all kinds of termination, so the radar can get into a defined state
#ifdef __linux__
    UnixSignalWatcher sigwatch;
    sigwatch.watchForSignal(SIGINT);
    sigwatch.watchForSignal(SIGTERM);
    sigwatch.watchForSignal(SIGQUIT);
    sigwatch.watchForSignal(SIGHUP);
    QObject::connect(&sigwatch, SIGNAL(unixSignal(int)), &a, SLOT(quit()));
#endif

    MainWindow w;
    Radar r;

    // Setup the radar before we move it into own thread
    if (!tryConnect(r))
        return ERROR_STARTUP_CONNECTION_FAILED;

    if (!tryAddingEndpoints(r))
        return ERROR_STARTUP_ADDING_ENDPOINTS_FAILED;

    if (!trySettingUpFrameTrigger(r))
        return ERROR_STARTUP_FRAMETRIGGER_SETUP_FAILED;


    // Move the radar object into another thread, so the main thread with the gui won't block
    QThread* t = new QThread();
    r.moveToThread(t);

    // Signal slot connections
    qRegisterMetaType<Targets_t>("Targets_t");
    qRegisterMetaType<DataPoints_t>("DataPoints_t");
    QObject::connect(&r, &Radar::timeDataChanged, &w, &MainWindow::updateTimeData);
    QObject::connect(&r, &Radar::rangeDataChanged, &w, &MainWindow::updateRangeData);
    QObject::connect(&r, &Radar::targetDataChanged, &w, &MainWindow::updateTargetData);
    QObject::connect(t, &QThread::started, &r, &Radar::doMeasurement);
#ifdef _WIN32
    QObject::connect(&w, &MainWindow::closed, &r, &Radar::disconnect, Qt::DirectConnection);
#endif

    // Start the thread
    t->start();

    // Main thread execution continues...
    w.show();

#ifdef __linux__
    auto ret = a.exec();
    r.disconnect();
    return ret;
#elif _WIN32
    return a.exec();
#endif
}
