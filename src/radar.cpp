#include "radar.h"

#include <COMPort.h>
#include <EndpointCalibration.h>
#include <EndpointRadarAdcxmc.h>
#include <EndpointRadarBase.h>
#include <EndpointRadarDoppler.h>
#include <EndpointRadarErrorCodes.h>
#include <EndpointRadarFmcw.h>
#include <EndpointRadarIndustrial.h>
#include <EndpointRadarP2G.h>
#include <Protocol.h>

#include <QDebug>
#include <QPointF>
#include <QThread>

// Constants
constexpr int STATE_RADAR_DISCONNECTED = -1;

// Callbacks
void CbReceivedFrameData(void* context, int32_t handle, uint8_t endpoint, const Frame_Info_t* frame_info);
void CbReceivedTargetData(void* context, int32_t handle, uint8_t endpoint, const  Target_Info_t* frame_info, uint8_t num_targets);


Radar::Radar(QObject *parent) : QObject(parent)
{
    m_handle = STATE_RADAR_DISCONNECTED;
    m_shutdown = false;
}

Radar::~Radar()
{}

bool Radar::connect()
{
    qDebug() << "Trying to connect to radar...";

    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
    {
        m_handle = protocol_connect(info.portName().toStdString().c_str());

        if (m_handle >= 0)
        {
            qDebug() << "Device found...";
            printSerialPortInformation(info);
            printFirmwareInformation();
            return true;
        }
    }

    qDebug() << "No devices found...";

    return false;
}

void Radar::disconnect()
{
    if (!m_shutdown)
        stopMeasurement();

    if (m_handle >= 0)
        protocol_disconnect(m_handle);

    qDebug() << "Device succesfully disconnected...";
}

bool Radar::addEndpoint(const EndpointType &endpoint)
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

bool Radar::setAutomaticFrameTrigger(bool enable, const EndpointType &endpoint, size_t interval_us)
{
    int res = 0;

    if (enable)
        res = ep_radar_base_set_automatic_frame_trigger(m_handle, m_endpoints[endpoint], interval_us);
    else
        res = ep_radar_base_set_automatic_frame_trigger(m_handle, m_endpoints[endpoint], 0);

    if (res < 0)
        return false;

    return true;
}

void Radar::doMeasurement()
{
    m_shutdown = false;
    ep_radar_base_set_callback_data_frame(CbReceivedFrameData, this);
    ep_targetdetect_set_callback_target_processing(CbReceivedTargetData, this);

    while(!m_shutdown)
    {
        ep_radar_base_get_frame_data(m_handle, m_endpoints[EndpointType::Base], 1);
        ep_targetdetect_get_targets(m_handle, m_endpoints[EndpointType::TargetDetection]);
    }
}

void Radar::stopMeasurement()
{
    m_shutdown = true;
}

void Radar::printSerialPortInformation(const QSerialPortInfo &info)
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

void Radar::printFirmwareInformation()
{
    Firmware_Information_t info;
    protocol_get_firmware_information(m_handle, &info);

    QString s = QObject::tr("Description: ") + info.description + "\n"
                + QObject::tr("Firmware: ") + QString::number(info.version_major) + "." + QString::number(info.version_minor);

    qDebug().noquote()  << s;
}

void Radar::printStatusCodeInformation(int code)
{
    qDebug() << "Status: " << protocol_get_status_code_description(m_handle, code);
}

void CbReceivedFrameData(void* context, int32_t handle, uint8_t endpoint, const Frame_Info_t* frame_info)
{
    QList<QPointF> re_rx1, im_rx1, re_rx2, im_rx2;

    for (uint32_t i = 0; i < 4 * frame_info->num_samples_per_chirp; i++)
    {
        if (i < frame_info->num_samples_per_chirp)
        {
            QPointF p(i, frame_info->sample_data[i]);
            re_rx1.push_back(p);
        }
        else if(i < 2 * frame_info->num_samples_per_chirp)
        {
            QPointF p(i % frame_info->num_samples_per_chirp, frame_info->sample_data[i]);
            im_rx1.push_back(p);
        }
        else if(i < 3 * frame_info->num_samples_per_chirp)
        {
            QPointF p(i % (2 * frame_info->num_samples_per_chirp), frame_info->sample_data[i]);
            re_rx2.push_back(p);
        }
        else
        {
            QPointF p(i % (3 * frame_info->num_samples_per_chirp), frame_info->sample_data[i]);
            im_rx2.push_back(p);
        }
    }

    emit ((Radar*)context)->timeDataChanged(re_rx1, im_rx1, re_rx2, im_rx2);
    emit ((Radar*)context)->rangeDataChanged(re_rx1);
}

void CbReceivedTargetData(void* context, int32_t handle, uint8_t endpoint, const  Target_Info_t* frame_info, uint8_t num_targets)
{
    QVector<Target_Info_t> vec;

    vec.reserve(num_targets);
    std::copy(frame_info, frame_info + num_targets, std::back_inserter(vec));

    emit ((Radar*)context)->targetDataChanged(vec);
}
