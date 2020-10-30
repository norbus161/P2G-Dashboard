#ifndef RADAR_H
#define RADAR_H

#include <QObject>
#include <QMap>
#include <QtSerialPort/QSerialPortInfo>

#include "endpoint_type.h"


class Radar : public QObject
{
    Q_OBJECT
public:
    explicit Radar(QObject *parent = nullptr);
    ~Radar();

    bool Connect();
    void Disconnect();

    bool AddEndpoint(EndpointType const & endpoint);
    bool EnableAutomaticFrameTrigger(EndpointType const & endpoint, size_t interval_us);
    bool DisableAutomaticFrameTrigger(EndpointType const & endpoint);

public slots:
    void DoMeasurement();

private:
    void PrintSerialPortInformation(QSerialPortInfo const & info);
    void PrintFirmwareInformation();
    void PrintStatusCodeInformation(int code);

private:
    int m_handle;
    QMap<EndpointType, int> m_endpoints;
};

#endif // RADAR_H
