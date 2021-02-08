#include "statusbar.h"

StatusBar::StatusBar(QWidget *parent) : QStatusBar(parent)
{
    m_firmware_description = "";
    m_firmware_version = "";
    m_serial_port = "";
    m_temperature = "";
    m_connection_state = false;
    setSizeGripEnabled(false);
    setVisible(false);
}

void StatusBar::updateFirmwareInformation(QString const & description, QString const & version)
{
    m_firmware_description = description;
    m_firmware_version = version;
    showMessage(getStatus());
}

void StatusBar::updateSerialPort(const QString &port)
{
    m_serial_port = port;
    showMessage(getStatus());
}

void StatusBar::updateTemperature(const QString &temperature)
{
    m_temperature = temperature;
    showMessage(getStatus());
}

void StatusBar::updateConnection(bool connected)
{
    m_connection_state = connected;
    showMessage(getStatus());
}

QString StatusBar::getStatus()
{
    QString msg = "Status: ";
    if (!m_connection_state)
    {
        msg.append("Disconnected");
    }
    else
    {
        msg.append(QString("Connected  |  Serial port: %1  |  Description: %2  |  Firmware: v%3  |  Temperature: %4°C")
                    .arg(m_serial_port)
                    .arg(m_firmware_description)
                    .arg(m_firmware_version)
                    .arg(m_temperature));
    }
    return msg;
}
