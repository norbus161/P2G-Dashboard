#include "radar.h"
#include "constants.h"

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
#include <QThread>

// Constants
constexpr auto STATE_RADAR_DISCONNECTED = -1;
constexpr auto LENGTH_SPLITTER = 50;

// Callbacks
void CbReceivedFrameData(void* context, int32_t handle, uint8_t endpoint, const Frame_Info_t* frame_info);
void CbReceivedTargetData(void* context, int32_t handle, uint8_t endpoint, const  Target_Info_t* frame_info, uint8_t num_targets);
void CbTemperature(void *context, int32_t handle, uint8_t endpoint, uint8_t temp_sensor, int32_t temperature);


Radar::Radar(QObject *parent) : QObject(parent)
{
    m_handle = STATE_RADAR_DISCONNECTED;
    m_shutdown = false;
}

Radar::~Radar()
{}

bool Radar::connect()
{
    QMutexLocker locker(&m);

    qDebug() << "Trying to connect to radar...";

    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
    {
        #ifdef __linux__
            m_handle = protocol_connect(("/dev/" + info.portName()).toStdString().c_str());
        #elif _WIN32
            m_handle = protocol_connect(info.portName().toStdString().c_str());
        #endif

        if (m_handle >= 0)
        {
            qDebug() << "Device found...";
            printSerialPortInformation(info);
            printFirmwareInformation();
            setCallbackFunctions();
            m_shutdown = false;
            return true;
        }
    }

    qDebug() << "No devices found...";

    return false;
}

void Radar::disconnect()
{
    QMutexLocker locker(&m);

    if (!m_shutdown)
        m_shutdown = true;

    protocol_do_firmware_reset(m_handle);

    if (m_handle >= 0)
        protocol_disconnect(m_handle);

    qDebug() << "Device succesfully disconnected...";
}

bool Radar::addEndpoint(const EndpointType_t &endpoint)
{
    auto const nr_of_endpoints = protocol_get_num_endpoints(m_handle);

    for (int i = 1; i <= nr_of_endpoints; ++i)
    {
        switch(endpoint)
        {
            case EndpointType_t::Calibration:
                if (ep_calibration_is_compatible_endpoint(m_handle, i) == 0)
                {
                    m_endpoints.insert(endpoint, i);
                    return true;
                }
            break;

            case EndpointType_t::AdcXmc:
                if (ep_radar_adcxmc_is_compatible_endpoint(m_handle, i) == 0)
                {
                    m_endpoints.insert(endpoint, i);
                    return true;
                }
            break;

            case EndpointType_t::Base:
                if (ep_radar_base_is_compatible_endpoint(m_handle, i) == 0)
                {
                    m_endpoints.insert(endpoint, i);
                    return true;
                }
            break;

            case EndpointType_t::Doppler:
                if (ep_radar_doppler_is_compatible_endpoint(m_handle, i) == 0)
                {
                    m_endpoints.insert(endpoint, i);
                    return true;
                }
            break;

            case EndpointType_t::Fmcw:
                if (ep_radar_fmcw_is_compatible_endpoint(m_handle, i) == 0)
                {
                    m_endpoints.insert(endpoint, i);
                    return true;
                }
            break;

            case EndpointType_t::Industrial:
                if (ep_radar_industrial_is_compatible_endpoint(m_handle, i) == 0)
                {
                    m_endpoints.insert(endpoint, i);
                    return true;
                }
            break;

            case EndpointType_t::P2G:
                if (ep_radar_p2g_is_compatible_endpoint(m_handle, i) == 0)
                {
                    m_endpoints.insert(endpoint, i);
                    return true;
                }
            break;

            case EndpointType_t::TargetDetection:
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

bool Radar::setAutomaticFrameTrigger(bool enable, const EndpointType_t &endpoint, size_t interval_us)
{
    auto ret = 0;

    if (enable)
        ret = ep_radar_base_set_automatic_frame_trigger(m_handle, m_endpoints[endpoint], interval_us);
    else
        ret = ep_radar_base_set_automatic_frame_trigger(m_handle, m_endpoints[endpoint], 0);

    if (ret < 0)
        return false;

    return true;
}

void Radar::doMeasurement()
{
    while(true)
    {
        m.lock();
        if (m_shutdown)
            break;

        ep_radar_base_get_temperature(m_handle, m_endpoints[EndpointType_t::Base], 0);
        printStatusCodeInformation(ep_radar_base_get_frame_data(m_handle, m_endpoints[EndpointType_t::Base], 0));
        printStatusCodeInformation(ep_targetdetect_get_targets(m_handle, m_endpoints[EndpointType_t::TargetDetection]));
        m.unlock();

        QThread::msleep(RADAR_MEASUREMENT_PAUSE_TIME);
    }
    m.unlock();
}

void Radar::emitRangeDataSignal(const DataPoints_t &re_rx1, const DataPoints_t &im_rx1,
                                const DataPoints_t &re_rx2, const DataPoints_t &im_rx2)
{

    auto rx1 = m_signal_processor.calculateRangeData(re_rx1, im_rx1);
    auto rx2 = m_signal_processor.calculateRangeData(re_rx2, im_rx2);

    std::vector<float> data;

    for (auto i = 0; i < rx1.size(); i++)
        data.push_back(rx1[i].y());

    m_persistence.RunPersistence(data);

    std::vector<p1d::TPairedExtrema> extrema;
    m_persistence.GetPairedExtrema(extrema, 0.01);

    auto maximum = 0.0;
    DataPoints_t maxima;

    for(auto it = extrema.begin(); it != extrema.end(); it++)
    {
        auto x = rx1[(*it).MaxIndex].x();
        auto y = rx1[(*it).MaxIndex].y();

        maxima.append(QPointF(x, y));

        if (y > maximum)
            maximum = y;
    }

    emit rangeDataChanged(rx1, rx2, maxima, maximum);
}

void Radar::printSerialPortInformation(const QSerialPortInfo &info)
{
    qDebug() << QString("-").repeated(LENGTH_SPLITTER);
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
    qDebug() << QString("-").repeated(LENGTH_SPLITTER);
    Firmware_Information_t info;
    protocol_get_firmware_information(m_handle, &info);

    QString s = QObject::tr("Description: ")
                + info.description + "\n"
                + QObject::tr("Firmware: ")
                + QString::number(info.version_major) + "."
                + QString::number(info.version_minor) + "."
                + QString::number(info.version_build);

    qDebug().noquote()  << s;
}

void Radar::printStatusCodeInformation(int code)
{
    qDebug() << "Status: " << protocol_get_status_code_description(m_handle, code);
}

void Radar::setCallbackFunctions()
{
    ep_radar_base_set_callback_data_frame(CbReceivedFrameData, this);
    ep_targetdetect_set_callback_target_processing(CbReceivedTargetData, this);
    ep_radar_base_set_callback_temperature(CbTemperature, this);
}

void CbReceivedFrameData(void* context, int32_t handle, uint8_t endpoint, const Frame_Info_t* frame_info)
{
    if (frame_info == nullptr)
        return;

    DataPoints_t re_rx1, im_rx1, re_rx2, im_rx2;

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
    ((Radar*)context)->emitRangeDataSignal(re_rx1, im_rx1, re_rx2, im_rx2);
}

void CbReceivedTargetData(void* context, int32_t handle, uint8_t endpoint, const  Target_Info_t* target_info, uint8_t num_targets)
{
    if (target_info == nullptr)
        return;

    Targets_t vec;
    vec.reserve(num_targets);
    std::copy(target_info, target_info + num_targets, std::back_inserter(vec));

    emit ((Radar*)context)->targetDataChanged(vec);
}

void CbTemperature(void *context, int32_t handle, uint8_t endpoint, uint8_t temp_sensor, int32_t temperature)
{
    qDebug() << "Temperature: " << temperature/1000 << "°C";
}
