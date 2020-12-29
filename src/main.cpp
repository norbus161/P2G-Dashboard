#include <misc/types.h>
#include <misc/constants.h>
#include <misc/messagehandler.h>
#include <logic/radar/radar.h>
#include <gui/dashboard/dashboard.h>
#include <gui/statusbar/statusbar.h>
#include <gui/toolbar/toolbar.h>
#include <gui/settings/settings.h>
#include <gui/chart/timedata/timedatachart.h>
#include <gui/chart/rangedata/rangedatachart.h>
#include <gui/chart/targetdata/targetdatachart.h>
#ifdef __linux__
    #include "sigwatch.h"
#endif

#include <QApplication>
#include <QThread>


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

    // Instantiate all variables
    Radar radar;
    Dashboard dashboard;
    StatusBar statusbar(&dashboard);
    ToolBar toolbar(&dashboard);
    Settings settings(&dashboard);
    TimeDataChart timedata;
    RangeDataChart rangedata;
    TargetDataChart targetdata;

    // Setup the Mainwindow
    dashboard.setStatusbar(&statusbar);
    dashboard.setToolbar(&toolbar);
    dashboard.setSettings(&settings);
    dashboard.setChart(&timedata, ChartType_t::TimeData);
    dashboard.setChart(&rangedata, ChartType_t::RangeData);
    dashboard.setChart(&targetdata, ChartType_t::TargetData);

    // Connections: Toolbar --> Settings
    QObject::connect(&toolbar, &ToolBar::settingsClicked, &settings, &Settings::requestAll);
    QObject::connect(&toolbar, &ToolBar::settingsClicked, &settings, &Settings::show);

    // Connections: Settings --> Radar
    qRegisterMetaType<Frame_Format_t>("Frame_Format_t");
    QObject::connect(&settings, &Settings::requestFrameFormat, &radar, &Radar::getFrameFormat, Qt::DirectConnection);
    QObject::connect(&settings, &Settings::frameFormatChanged, &radar, &Radar::setFrameFormat, Qt::DirectConnection);
    qRegisterMetaType<DSP_Settings_t>("DSP_Settings_t");
    QObject::connect(&settings, &Settings::requestDspSettings, &radar, &Radar::getDspSettings, Qt::DirectConnection);
    QObject::connect(&settings, &Settings::dspSettingsChanged, &radar, &Radar::setDspSettings, Qt::DirectConnection);

    // Connections: Radar --> Settings
    QObject::connect(&radar, &Radar::frameFormatChanged, &settings, &Settings::responseFrameFormat);
    QObject::connect(&radar, &Radar::dspSettingsChanged, &settings, &Settings::responseDspSettings);

    // Connections: Radar --> Statusbar
    QObject::connect(&radar, &Radar::connectionChanged, &statusbar, &StatusBar::updateConnection);
    QObject::connect(&radar, &Radar::firmwareInformationChanged, &statusbar, &StatusBar::updateFirmwareInformation);
    QObject::connect(&radar, &Radar::temperatureChanged, &statusbar, &StatusBar::updateTemperature);
    QObject::connect(&radar, &Radar::serialPortChanged, &statusbar, &StatusBar::updateSerialPort);

    // Connections: Radar --> Charts
    qRegisterMetaType<Targets_t>("Targets_t");
    qRegisterMetaType<DataPoints_t>("DataPoints_t");
    QObject::connect(&radar, &Radar::timeDataChanged, &timedata, &TimeDataChart::update);
    QObject::connect(&radar, &Radar::rangeDataChanged, &rangedata, &RangeDataChart::update);
    QObject::connect(&radar, &Radar::targetDataChanged, &targetdata, &TargetDataChart::update);

    // Try to setup the radar sensor
    if (!tryConnect(radar))
        return ERROR_STARTUP_CONNECTION_FAILED;
    if (!tryAddingEndpoints(radar))
        return ERROR_STARTUP_ADDING_ENDPOINTS_FAILED;
    if (!trySettingUpFrameTrigger(radar))
        return ERROR_STARTUP_FRAMETRIGGER_SETUP_FAILED;

    // Move the radar object into another thread, so the main thread with the gui won't block
    QThread* thread = new QThread();
    radar.moveToThread(thread);

    // Connections: Thread --> Radar
    QObject::connect(thread, &QThread::started, &radar, &Radar::doMeasurement);

#ifdef _WIN32
    // Connections: Dashboard --> Radar
    QObject::connect(&dashboard, &Dashboard::closed, &radar, &Radar::disconnect, Qt::DirectConnection);
#endif

    // Start the thread
    thread->start();

    // Main thread execution continues...
    dashboard.show();

#ifdef __linux__
    auto ret = a.exec();
    radar.disconnect();
    return ret;
#elif _WIN32
    return a.exec();
#endif
}
