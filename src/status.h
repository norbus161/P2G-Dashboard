#ifndef STATUS_H
#define STATUS_H

#include "Protocol.h"

#include <QString>
#include <QObject>

class Status : public QObject
{
    Q_OBJECT

public:
    explicit Status(QObject *parent = nullptr);
    QString get();

public slots:
    void updateFirmwareInformation(Firmware_Information_t const & info);
    void updateSerialPort(QString const & port);
    void updateTemperature(QString const & temperature);
    void updateConnection(bool connected);

signals:
    void changed();

private:
    Firmware_Information_t m_firmware_info;
    QString m_serial_port;
    QString m_temperature;
    bool m_connection_state;
};

#endif // STATUS_H
