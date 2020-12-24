#include "status.h"

Status::Status(QObject *parent) : QObject(parent)
{
    m_firmware_info = {};
    m_serial_port = "";
    m_temperature = 0.0;
    m_connection_state = false;
}

void Status::updateFirmwareInformation(const Firmware_Information_t &info)
{
    m_firmware_info = info;
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
        msg += "Disconnected";
    }
    else
    {
        auto t = QString(m_firmware_info.description);
        auto f = QString("%1.%2.%3").arg(m_firmware_info.version_major).arg(m_firmware_info.version_minor).arg(m_firmware_info.version_build);
        auto s = QString("Connected  |  Serial port: %1  |  Description: %2  |  Firmware: v%3  |  Temperature: %4°C").arg(m_serial_port).arg(t).arg(f).arg(m_temperature);
        msg.append(s);
    }
    return msg;
}
