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


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

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

    auto attempts = STARTUP_CONNECTION_ATTEMPS;
    while (attempts > 0 && !r.connect())
    {
        QThread::msleep(STARTUP_CONNECTION_PAUSE_TIME);
        attempts--;
    }

    if (attempts == 0)
    {
        qCritical() << "Aborted: Failed to connect to radar sensor. ";
        return ERROR_STARTUP_CONNECTION_FAILED;
    }

    // Add the necassery endpoints
    qDebug() << "Adding base endpoint: " << r.addEndpoint(EndpointType_t::Base);
    qDebug() << "Adding target detection endpoint: " << r.addEndpoint(EndpointType_t::TargetDetection);

    // Disable automatic frame trigger
    r.setAutomaticFrameTrigger(false, EndpointType_t::Base, 0);
    r.setAutomaticFrameTrigger(false, EndpointType_t::TargetDetection, 0);

    // Move the radar object into another thread, so the gui thread won't block
    QThread* t = new QThread();
    r.moveToThread(t);

    // Signal slot connections
    qRegisterMetaType<DataPoints_t>("DataPoints_t");
    qRegisterMetaType<Targets_t>("Targets_t");
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
