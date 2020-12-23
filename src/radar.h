#ifndef RADAR_H
#define RADAR_H

#include "types.h"
#include "signalprocessor.h"

#include <persistence1d.hpp>
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

public slots:
    void disconnect();
    void doMeasurement();
    void emitRangeDataSignal(DataPoints_t const & re_rx1, DataPoints_t const & im_rx1,
                             DataPoints_t const & re_rx2, DataPoints_t const & im_rx2);
signals:
    void timeDataChanged(DataPoints_t const & re_rx1, DataPoints_t const & im_rx1,
                         DataPoints_t const & re_rx2, DataPoints_t const & im_rx2);
    void rangeDataChanged(DataPoints_t const & rx1, DataPoints_t const & rx2,
                          DataPoints_t const & maxima, double const & max_y);
    void targetDataChanged(Targets_t const & data);

private:
    void printSerialPortInformation(QSerialPortInfo const & info);
    void printFirmwareInformation();
    void printStatusCodeInformation(int code);
    void setCallbackFunctions();

private:
    int m_handle;
    bool m_shutdown;
    QRecursiveMutex m;
    QMap<EndpointType_t, int> m_endpoints;
    SignalProcessor m_signal_processor;
    p1d::Persistence1D m_persistence;
};

#endif // RADAR_H
