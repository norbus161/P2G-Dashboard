#ifndef STATUS_H
#define STATUS_H

#include "Protocol.h"

#include <QObject>

class Status : public QObject
{
    Q_OBJECT

public:
    explicit Status(QObject *parent = nullptr);
    QString get();

public slots:
    void updateFirmwareInformation(QString const & description, QString const & version);
    void updateSerialPort(QString const & port);
    void updateTemperature(QString const & temperature);
    void updateConnection(bool connected);

signals:
    void changed();

private:
    QString m_firmware_description;
    QString m_firmware_version;
    QString m_serial_port;
    QString m_temperature;
    bool m_connection_state;
};

#endif // STATUS_H
