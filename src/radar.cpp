#include "radar.h"
#include "3rdparty/ComLib_C_Interface/include/COMPort.h"
#include "3rdparty/ComLib_C_Interface/include/EndpointCalibration.h"
#include "3rdparty/ComLib_C_Interface/include/EndpointRadarAdcxmc.h"
#include "3rdparty/ComLib_C_Interface/include/EndpointRadarBase.h"
#include "3rdparty/ComLib_C_Interface/include/EndpointRadarDoppler.h"
#include "3rdparty/ComLib_C_Interface/include/EndpointRadarErrorCodes.h"
#include "3rdparty/ComLib_C_Interface/include/EndpointRadarFmcw.h"
#include "3rdparty/ComLib_C_Interface/include/EndpointRadarIndustrial.h"
#include "3rdparty/ComLib_C_Interface/include/EndpointRadarP2G.h"
#include "3rdparty/ComLib_C_Interface/include/EndpointTargetDetection.h"
#include "3rdparty/ComLib_C_Interface/include/Protocol.h"

#include <QDebug>

constexpr int STATE_RADAR_DISCONNECTED = -1;

Radar::Radar(QObject *parent) : QObject(parent)
{
    m_handle = STATE_RADAR_DISCONNECTED;
}

Radar::~Radar()
{}

bool Radar::Connect()
{
    qDebug() << "Trying to connect to radar...";
    qDebug() << "Checking serial ports...";

    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
    {
        m_handle = protocol_connect(info.portName().toStdString().c_str());

        if (m_handle >= 0)
        {
            qDebug() << "Device found...";
            PrintSerialPortInformation(info);
            PrintFirmwareInformation();
            return true;
        }
    }

    qDebug() << "No devices found...";

    return false;
}

void Radar::Disconnect()
{
    if (m_handle >= 0)
    {
        protocol_disconnect(m_handle);
    }
}

bool Radar::AddEndpoint(const EndpointType &endpoint)
{
    auto const nr_of_endpoints = protocol_get_num_endpoints(m_handle);

    for (int i = 1; i <= nr_of_endpoints; ++i)
    {
        switch(endpoint)
        {
            case EndpointType::Calibration:
            if (ep_calibration_is_compatible_endpoint(m_handle, i) == 0)
            {
                m_endpoints.insert(endpoint, i);
                return true;
            }
            break;

            case EndpointType::AdcXmc:
            if (ep_radar_adcxmc_is_compatible_endpoint(m_handle, i) == 0)
            {
                m_endpoints.insert(endpoint, i);
                return true;
            }
            break;

            case EndpointType::Base:
            if (ep_radar_base_is_compatible_endpoint(m_handle, i) == 0)
            {
                m_endpoints.insert(endpoint, i);
                return true;
            }
            break;

            case EndpointType::Doppler:
            if (ep_radar_doppler_is_compatible_endpoint(m_handle, i) == 0)
            {
                m_endpoints.insert(endpoint, i);
                return true;
            }
            break;

            case EndpointType::Fmcw:
            if (ep_radar_fmcw_is_compatible_endpoint(m_handle, i) == 0)
            {
                m_endpoints.insert(endpoint, i);
                return true;
            }
            break;

            case EndpointType::Industrial:
            if (ep_radar_industrial_is_compatible_endpoint(m_handle, i) == 0)
            {
                m_endpoints.insert(endpoint, i);
                return true;
            }
            break;

            case EndpointType::P2G:
            if (ep_radar_p2g_is_compatible_endpoint(m_handle, i) == 0)
            {
                m_endpoints.insert(endpoint, i);
                return true;
            }
            break;

            case EndpointType::TargetDetection:
            if (ep_targetdetect_is_compatible_endpoint(m_handle, i) == 0)
            {
                m_endpoints.insert(endpoint, i);
                return true;
            }
            break;
        }
    }

    return false;
}

bool Radar::EnableAutomaticFrameTrigger(const EndpointType &endpoint, size_t interval_us)
{
    int res = 0;
    if ((res = ep_radar_base_set_automatic_frame_trigger(m_handle, m_endpoints[endpoint], interval_us)) < 0)
    {
        return false;
    }

    PrintStatusCodeInformation(res);
    return true;
}

bool Radar::DisableAutomaticFrameTrigger(const EndpointType &endpoint)
{
    int res = 0;
    if ((res = ep_radar_base_set_automatic_frame_trigger(m_handle, m_endpoints[endpoint], 0)) < 0)
    {
        return false;
    }

    PrintStatusCodeInformation(res);
    return true;
}

void Radar::PrintSerialPortInformation(const QSerialPortInfo &info)
{
    QString s = QObject::tr("Port: ") + info.portName() + "\n"
                + QObject::tr("Location: ") + info.systemLocation() + "\n"
                + QObject::tr("Description: ") + info.description() + "\n"
                + QObject::tr("Manufacturer: ") + info.manufacturer() + "\n"
                + QObject::tr("Serial number: ") + info.serialNumber() + "\n"
                + QObject::tr("Vendor Identifier: ") + (info.hasVendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : QString()) + "\n"
                + QObject::tr("Product Identifier: ") + (info.hasProductIdentifier() ? QString::number(info.productIdentifier(), 16) : QString());

    qDebug().noquote()  << s;
}

void Radar::PrintFirmwareInformation()
{
    Firmware_Information_t info;
    protocol_get_firmware_information(m_handle, &info);

    QString s = QObject::tr("Description: ") + info.description + "\n"
                + QObject::tr("Firmware: ") + QString::number(info.version_major) + "." + QString::number(info.version_minor);

    qDebug().noquote()  << s;
}

void Radar::PrintStatusCodeInformation(int code)
{
    qDebug() << "Status: " << protocol_get_status_code_description(m_handle, code);
}
