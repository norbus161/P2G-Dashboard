#include "status.h"

Status::Status(QObject *parent) : QObject(parent)
{
    m_firmware_description = "";
    m_firmware_version = "";
    m_serial_port = "";
    m_temperature = "";
    m_connection_state = false;
}

void Status::updateFirmwareInformation(QString const & description, QString const & version)
{
    m_firmware_description = description;
    m_firmware_version = version;
    emit changed();
}

void Status::updateSerialPort(const QString &port)
{
    m_serial_port = port;
    emit changed();
}

void Status::updateTemperature(const QString &temperature)
{
    m_temperature = temperature;
    emit changed();
}

void Status::updateConnection(bool connected)
{
    m_connection_state = connected;
    emit changed();
}

QString Status::get()
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
