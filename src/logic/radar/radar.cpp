#include "radar.h"

#include <misc/constants.h>
#include <COMPort.h>
#include <EndpointCalibration.h>
#include <EndpointRadarAdcxmc.h>
#include <EndpointRadarBase.h>
#include <EndpointRadarDoppler.h>
#include <EndpointRadarErrorCodes.h>
#include <EndpointRadarFmcw.h>
#include <EndpointRadarIndustrial.h>
#include <EndpointRadarP2G.h>
#include <QDebug>
#include <QThread>

// Constants
constexpr auto STATE_RADAR_DISCONNECTED = -1;

// Callbacks
void CbReceivedFrameData(void* context, int32_t handle, uint8_t endpoint, const Frame_Info_t* frame_info);
void CbReceivedTargetData(void* context, int32_t handle, uint8_t endpoint, const  Target_Info_t* frame_info, uint8_t num_targets);
void CbTemperature(void *context, int32_t handle, uint8_t endpoint, uint8_t temp_sensor, int32_t temperature);
void CbGetFrameFormat(void *context, int32_t protocol_handle, uint8_t endpoint, const Frame_Format_t *frame_format);
void CbGetDspSettings(void *context, int32_t protocol_handle, uint8_t endpoint, const DSP_Settings_t *dsp_settings);

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

    qInfo() << "Trying to connect to radar...";

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
            qInfo() << "Device found.";
            printSerialPortInformation(info);
            printFirmwareInformation();
            setCallbackFunctions();
            m_shutdown = false;
            emit connectionChanged(true);
            return true;
        }
    }

    qInfo() << "No devices found...";

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

    emit connectionChanged(false);
    qInfo() << "Device succesfully disconnected.";
}

bool Radar::addEndpoint(const EndpointType_t &endpoint)
{
    auto const nr_of_endpoints = protocol_get_num_endpoints(m_handle);

    for (int i = 1; i <= nr_of_endpoints; ++i)
    {
        switch(endpoint)
        {
            case EndpointType_t::Calibration:
                if (getStatusCodeInformation("Add endpoint", ep_calibration_is_compatible_endpoint(m_handle, i)))
                {
                    m_endpoints.insert(endpoint, i);
                    return true;
                }
            break;

            case EndpointType_t::AdcXmc:
                if (getStatusCodeInformation("Add endpoint", ep_radar_adcxmc_is_compatible_endpoint(m_handle, i)))
                {
                    m_endpoints.insert(endpoint, i);
                    return true;
                }
            break;

            case EndpointType_t::Base:
                if (getStatusCodeInformation("Add endpoint", ep_radar_base_is_compatible_endpoint(m_handle, i)))
                {
                    m_endpoints.insert(endpoint, i);
                    return true;
                }
            break;

            case EndpointType_t::Doppler:
                if (getStatusCodeInformation("Add endpoint", ep_radar_doppler_is_compatible_endpoint(m_handle, i)))
                {
                    m_endpoints.insert(endpoint, i);
                    return true;
                }
            break;

            case EndpointType_t::Fmcw:
                if (getStatusCodeInformation("Add endpoint", ep_radar_fmcw_is_compatible_endpoint(m_handle, i)))
                {
                    m_endpoints.insert(endpoint, i);
                    return true;
                }
            break;

            case EndpointType_t::Industrial:
                if (getStatusCodeInformation("Add endpoint", ep_radar_industrial_is_compatible_endpoint(m_handle, i)))
                {
                    m_endpoints.insert(endpoint, i);
                    return true;
                }
            break;

            case EndpointType_t::P2G:
                if (getStatusCodeInformation("Add endpoint", ep_radar_p2g_is_compatible_endpoint(m_handle, i)))
                {
                    m_endpoints.insert(endpoint, i);
                    return true;
                }
            break;

            case EndpointType_t::TargetDetection:
                if (getStatusCodeInformation("Add endpoint", ep_targetdetect_is_compatible_endpoint(m_handle, i)))
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
    auto ret = false;

    if (enable)
        ret = getStatusCodeInformation("Enable automatic frame trigger", ep_radar_base_set_automatic_frame_trigger(m_handle, m_endpoints[endpoint], interval_us));
    else
        ret = getStatusCodeInformation("Disable automatic frame trigger", ep_radar_base_set_automatic_frame_trigger(m_handle, m_endpoints[endpoint], 0));

    if (!ret)
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

        auto res = true;
        res &= getStatusCodeInformation("Get temperature", ep_radar_base_get_temperature(m_handle, m_endpoints[EndpointType_t::Base], 0));
        res &= getStatusCodeInformation("Get frame data", ep_radar_base_get_frame_data(m_handle, m_endpoints[EndpointType_t::Base], 0));
        res &= getStatusCodeInformation("Get target data", ep_targetdetect_get_targets(m_handle, m_endpoints[EndpointType_t::TargetDetection]));
        m.unlock();

        QThread::msleep(RADAR_MEASUREMENT_PAUSE_TIME);
    }
    m.unlock();
}

void Radar::getFrameFormat()
{
    QMutexLocker locker(&m);
    getStatusCodeInformation("Get frame format", ep_radar_base_get_frame_format(m_handle, m_endpoints[EndpointType_t::Base]));
}

void Radar::setFrameFormat(const Frame_Format_t &frame_format)
{
    QMutexLocker locker(&m);
    getStatusCodeInformation("Set frame format", ep_radar_base_set_frame_format(m_handle, m_endpoints[EndpointType_t::Base], &frame_format));
}

void Radar::getDspSettings()
{
    QMutexLocker locker(&m);
    getStatusCodeInformation("Get DSP settings", ep_targetdetect_get_dsp_settings(m_handle, m_endpoints[EndpointType_t::TargetDetection]));
}

void Radar::setDspSettings(const DSP_Settings_t &dsp_settings)
{
    QMutexLocker locker(&m);
    getStatusCodeInformation("Set DSP settings", ep_targetdetect_set_dsp_settings(m_handle, m_endpoints[EndpointType_t::TargetDetection], &dsp_settings));
}

void Radar::emitRangeDataSignal(const DataPoints_t &re_rx1, const DataPoints_t &im_rx1, const DataPoints_t &re_rx2, const DataPoints_t &im_rx2)
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
    qInfo() << "Port: " <<  info.portName();
    qInfo() << "Vendor Identifier: " << (info.hasVendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : QString());
    qInfo() << "Product Identifier: " << (info.hasProductIdentifier() ? QString::number(info.productIdentifier(), 16) : QString());
    emit serialPortChanged(info.portName());
}

void Radar::printFirmwareInformation()
{
    Firmware_Information_t info;
    if (getStatusCodeInformation("Get firmware information" ,protocol_get_firmware_information(m_handle, &info)))
    {
        QString description = info.description;
        QString version = QString("%1.%2.%3").arg(info.version_major).arg(info.version_minor).arg(info.version_build);

        qInfo() << "Description: " << description;
        qInfo() << "Firmware: " << version;
        emit firmwareInformationChanged(description, version);
    }
}

bool Radar::getStatusCodeInformation(QString const & origin, int code)
{
    auto ret = protocol_get_status_code_description(m_handle, code);

    if ((code & 0xFFFF) != PROTOCOL_STATUS_OK)
    {
        qWarning() << origin << ":" << ret;
        return false;
    }

    qInfo() << origin << ":" << ret;
    return true;
}

void Radar::setCallbackFunctions()
{
    qInfo() << "Setting callback functions...";

    ep_radar_base_set_callback_data_frame(CbReceivedFrameData, this);
    ep_targetdetect_set_callback_target_processing(CbReceivedTargetData, this);
    ep_radar_base_set_callback_temperature(CbTemperature, this);
    ep_radar_base_set_callback_frame_format(CbGetFrameFormat, this);
    ep_targetdetect_set_callback_dsp_settings(CbGetDspSettings, this);
}

void CbReceivedFrameData(void* context, int32_t, uint8_t, const Frame_Info_t* frame_info)
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

void CbReceivedTargetData(void* context, int32_t, uint8_t, const  Target_Info_t* target_info, uint8_t num_targets)
{
    if (target_info == nullptr)
        return;

    Targets_t vec;
    vec.reserve(num_targets);
    std::copy(target_info, target_info + num_targets, std::back_inserter(vec));

    emit ((Radar*)context)->targetDataChanged(vec);
}

void CbTemperature(void * context, int32_t, uint8_t, uint8_t, int32_t temperature)
{
    if (temperature <= 0)
        return;

    auto temp = QString::number(temperature/1000.0,'f',2);
    emit ((Radar*)context)->temperatureChanged(temp);
}

void CbGetFrameFormat(void *context, int32_t, uint8_t, const Frame_Format_t *frame_format)
{
    if (frame_format == nullptr)
        return;

    emit ((Radar*)context)->frameFormatChanged(*frame_format);
}

void CbGetDspSettings(void *context, int32_t, uint8_t, const DSP_Settings_t *dsp_settings)
{
    if (dsp_settings == nullptr)
        return;

    emit ((Radar*)context)->dspSettingsChanged(*dsp_settings);
}
