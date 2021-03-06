#ifndef RADAR_H
#define RADAR_H

#include <misc/types.h>
#include <logic/signalprocessor/signalprocessor.h>

#include <persistence1d.hpp>
#include <Protocol.h>
#include <EndpointRadarBase.h>
#include <EndpointTargetDetection.h>
#include <QtSerialPort/QSerialPortInfo>
#include <QObject>
#include <QMap>
#include <QMutex>


class Radar : public QObject
{
    Q_OBJECT

public:
    explicit Radar(QObject *parent = nullptr);
    ~Radar();

    bool connect();
    bool addEndpoint(EndpointType_t const & endpoint);
    bool setAutomaticFrameTrigger(bool enable, EndpointType_t const & endpoint, size_t interval_us);
    void emitRangeDataSignal(DataPoints_t const & re_rx1, DataPoints_t const & im_rx1, DataPoints_t const & re_rx2, DataPoints_t const & im_rx2);

public slots:
    void disconnect();
    void doMeasurement();
    void getFrameFormat();
    void setFrameFormat(Frame_Format_t const & frame_format);
    void getDspSettings();
    void setDspSettings(DSP_Settings_t const & dsp_settings);

signals:
    void timeDataChanged(DataPoints_t const & re_rx1, DataPoints_t const & im_rx1, DataPoints_t const & re_rx2, DataPoints_t const & im_rx2);
    void rangeDataChanged(DataPoints_t const & rx1, DataPoints_t const & rx2, DataPoints_t const & maxima, double const & max_y);
    void targetDataChanged(Targets_t const & data);
    void firmwareInformationChanged(QString const & description, QString const & version);
    void serialPortChanged(QString const & port);
    void temperatureChanged(QString const & temperature);
    void connectionChanged(bool connected);
    void frameFormatChanged(Frame_Format_t const & frame_format);
    void dspSettingsChanged(DSP_Settings_t const & dsp_settings);

private:
    void printSerialPortInformation(QSerialPortInfo const & info);
    bool checkFirmwareInformation(QString const & version);
    bool getStatusCodeInformation(QString const & origin, int code);
    void setCallbackFunctions();

private:
    int m_handle;
    bool m_shutdown;
    QMutex m;
    QMap<EndpointType_t, int> m_endpoints;
    SignalProcessor m_signal_processor;
    p1d::Persistence1D m_persistence;
};

#endif // RADAR_H
