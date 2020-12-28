#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QStatusBar>

class StatusBar : public QStatusBar
{
    Q_OBJECT

public:
    explicit StatusBar(QWidget *parent = nullptr);

public slots:
    void updateFirmwareInformation(QString const & description, QString const & version);
    void updateSerialPort(QString const & port);
    void updateTemperature(QString const & temperature);
    void updateConnection(bool connected);

private:
    QString getStatus();

private:
    QString m_firmware_description;
    QString m_firmware_version;
    QString m_serial_port;
    QString m_temperature;
    bool m_connection_state;
};

#endif // STATUSBAR_H
