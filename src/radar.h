#ifndef RADAR_H
#define RADAR_H

#include "endpoint_type.h"

#include <EndpointRadarBase.h>
#include <EndpointTargetDetection.h>

#include <QtSerialPort/QSerialPortInfo>
#include <QObject>
#include <QVector>
#include <QList>
#include <QMap>


class Radar : public QObject
{
    Q_OBJECT
public:
    explicit Radar(QObject *parent = nullptr);
    ~Radar();

    bool connect();
    bool addEndpoint(EndpointType const & endpoint);
    bool setAutomaticFrameTrigger(bool enable, EndpointType const & endpoint, size_t interval_us);

public slots:
    void disconnect();
    void doMeasurement();
    void stopMeasurement();

signals:
    void frameDataChanged(QList<QPointF> const & re_rx1, QList<QPointF> const & im_rx1,
                          QList<QPointF> const & re_rx2, QList<QPointF> const & im_rx2);
    void targetDataChanged(QVector<Target_Info_t> const & data);

private:
    void printSerialPortInformation(QSerialPortInfo const & info);
    void printFirmwareInformation();
    void printStatusCodeInformation(int code);

private:
    int m_handle;
    bool m_shutdown;
    QMap<EndpointType, int> m_endpoints;
};

#endif // RADAR_H
