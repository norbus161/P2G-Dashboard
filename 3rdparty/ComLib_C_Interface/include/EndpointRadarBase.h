/**
 * \file EndpointRadarBase.h
 *
 * \brief This file defines the API to communicate with Radar Base Endpoints
 *        in Infineon sensor devices.
 *
 * The module EndpointRadarBase.c provides functions to communicate with
 * Infineon radar sensor devices. The user must use the function provided by
 * the module \ref Protocol.c to open a connection to a radar sensor device
 * and query information about the endpoints in the device.
 *
 * The function \ref ep_radar_base_is_compatible_endpoint can be used to check
 * each endpoint if it is compatible with this API. If it is the types and
 * functions in from this module can be used to send parameters to the
 * endpoint and query the current settings and radar frame data.
 *
 * All data received from the connected device is returned through callbacks.
 * For each message type that can be received from the device, a separate
 * callback function type is defined. The user must register the callback
 * function for each message type. If no callback function is registered the
 * received message is ignored. Additionally the user can provide a data
 * context pointer for each callback function that will be forwarded with any
 * call to that function.
 */

/* ===========================================================================
** Copyright (C) 2016-2019 Infineon Technologies AG
** All rights reserved.
** ===========================================================================
**
** ===========================================================================
** This document contains proprietary information of Infineon Technologies AG.
** Passing on and copying of this document, and communication of its contents
** is not permitted without Infineon's prior written authorisation.
** ===========================================================================
*/

#ifndef ENDPOINTRADARBASE_H_INCLUDED
#define ENDPOINTRADARBASE_H_INCLUDED

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/
#include <stdint.h>
#include "EndpointRadarErrorCodes.h"

/* Enable C linkage if header is included in C++ files */
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * \defgroup EndpointRadarBase EndpointRadarBase
 *
 * \brief EndpointRadarBase documentation.
 *
 * @{
 */

/*
==============================================================================
   3. TYPES
==============================================================================
*/

/**
 * \brief This enumeration type lists the possible formats of sampled radar
 *        data.
 *
 * The format of sampled radar data acquired by the driver and retrieved
 * through a struct of type \ref Frame_Info_t by calling
 * \ref ep_radar_base_get_frame_data depends on the radar sensor hardware.
 *
 * The radar front end comprises a mixer that demodulates the RF signal
 * received through the connected antennas down to the base band, where it is
 * captured by an Analog/Digital Converter (ADC). A RF mixer can output two
 * signals with a phase shift of 90?, called I-signal and Q-signal. These two
 * signals can be considered to be real and imaginary part of a complex valued
 * signal. Depending on the sensor hardware, the sensor may be able to capture
 * only one or both signals with its ADCs.
 *
 * If only one signal is captured, the radar frame data is real valued, so a
 * block of N samples consists of N values. If both signals are captured, a
 * block of N samples consists of 2*N values, which may be interleaved (each
 * real value is followed by the according imaginary value) or straight (N real
 * samples followed by N imaginary samples).
 *
 */
typedef enum
{
    EP_RADAR_BASE_RX_DATA_REAL                = 0, /**< The frame data
                                                        contains only I or Q
                                                        signal. */
    EP_RADAR_BASE_RX_DATA_COMPLEX             = 1, /**< The frame data
                                                        contains I and Q
                                                        signals in separate
                                                        data blocks. */
    EP_RADAR_BASE_RX_DATA_COMPLEX_INTERLEAVED = 2  /**< The frame data
                                                        contains I and Q
                                                        signals in one
                                                        interleaved data
                                                        block. */
} Rx_Data_Format_t;

/**
 * \brief This enumeration type lists the possible signals parts formats of
 *        sampled radar data.
 *
 * If the device is capable to capture complex signal data (I and Q), the
 * values of this enumeration allow to chose between the full complex signal,
 * or just the real or imaginary signal part.
 *
 * A value of this type is part of the struct \ref Frame_Format_t.
 */
typedef enum
{
    EP_RADAR_BASE_SIGNAL_ONLY_I  = 0, /**< Only the I signal is captured
                                           during radar data frame
                                           acquisition. */
    EP_RADAR_BASE_SIGNAL_ONLY_Q  = 1, /**< Only the Q signal is captured
                                           during radar data frame
                                           acquisition. */
    EP_RADAR_BASE_SIGNAL_I_AND_Q = 2  /**< Both, I and Q signal are captured
                                           as a complex signal during radar
                                           data frame acquisition. */
} Signal_Part_t;

/**
 * \brief This struct holds the version of the radar driver behind the end
 *        point.
 *
 * Use \ref ep_radar_base_get_driver_version to query the driver version
 * information. When the sensor board sends version information, a structure
 * of this type is returned through the callback
 * \ref Callback_Driver_Version_t.
 */
typedef struct
{
    uint8_t major;     /**< The major version number of the driver. */
    uint8_t minor;     /**< The minor version number of the driver. */
    uint8_t revision;  /**< The revision number of the driver. */
} Driver_Version_t;

/**
 * \brief This structure holds information about a device and its
 *        capabilities.
 *
 * Use \ref ep_radar_base_get_device_info to query the device information. When the
 * sensor board sends device information, a structure of this type is returned
 * through the callback \ref Callback_Device_Info_t.
 */
typedef struct
{
    const char*      description;           /**< A pointer to a null
                                                 terminated string holding a
                                                 human readable description of
                                                 the device. */
    uint32_t         min_rf_frequency_kHz;  /**< The minimum RF frequency in
                                                 kHz the sensor device can
                                                 emit. */
    uint32_t         max_rf_frequency_kHz;  /**< The maximum RF frequency in
                                                 kHz the sensor device can
                                                 emit. */
    uint8_t          num_tx_antennas;       /**< The number of RF antennas
                                                 used for transmission. */
    uint8_t          num_rx_antennas;       /**< The number of RF antennas
                                                 used for reception. */
    uint8_t          max_tx_power;          /**< The amount of RF transmission
                                                 power can be controlled in
                                                 the range of
                                                 0 ... max_tx_power. */
    uint8_t          num_temp_sensors;      /**< The number of available
                                                 temperature sensors. */
    uint8_t          major_version_hw;      /**< The major version number of
                                                 the sensor hardware. */
    uint8_t          minor_version_hw;      /**< The minor version number of
                                                 the sensor hardware. */
    uint8_t          interleaved_rx;        /**< If this is 0, the radar data
                                                 of multiple RX antennas is
                                                 stored in consecutive data
                                                 blocks, where each block
                                                 holds data of one antenna. If
                                                 this is non-zero, the radar
                                                 data of multiple RX antennas
                                                 is stored in one data block,
                                                 where for each point in time
                                                 the samples from all RX
                                                 antennas are stored
                                                 consecutively before the data
                                                 of the next point in time
                                                 follows. */
    Rx_Data_Format_t data_format;           /**< If the device can acquire
                                                 complex data, this indicates
                                                 if real and imaginary part
                                                 are interleaved or not. */
} Device_Info_t;

/**
 * \brief This struct holds the parameters that define the structure of a
 *        radar data frame.
 *
 * A struct of this type must be passed to
 * \ref ep_radar_base_set_frame_format to change the frame format. When the
 * sensor board sends the current frame format, a structure of this type is
 * returned through the callback \ref Callback_Frame_Format_t. Use
 * \ref ep_radar_base_get_frame_format to query the current RF settings.
 */
typedef struct
{
    uint32_t      num_samples_per_chirp; /**< The number of samples captured
                                              during each chirp. This value
                                              affects the duration of a single
                                              chirp. This must not be 0. */
    uint32_t      num_chirps_per_frame;  /**< The number of chirps that are
                                              processed consecutively in a
                                              frame. Must not be 0. */
    uint8_t       rx_mask;               /**< Each available RX antenna is
                                              represented by a bit in this
                                              mask. If a bit is set, the IF
                                              signal received through the
                                              according RX antenna is captured
                                              during chirp processing. */
    Signal_Part_t eSignalPart;           /**< The part of a complex signal to
                                              be captured. */
} Frame_Format_t;

/**
 * \brief This struct holds all information about a single frame of radar
 *        data.
 *
 * A structure of this type is returned through the callback
 * \ref Callback_Data_Frame_t when \ref ep_radar_base_get_frame_data is
 * called.
 *
 * The data type of samples in sample_data depends on the ADC resolution. If
 * adc_resolution <= 8, sample_data points to an array of uint8_t. If
 * 8 < adc_resolution <= 16, sample_data points to an array of uint16_t.
 * Higher ADC resolutions are currently not supported.
 *
 * If the frame contains more than one chirp, the chirps are stored in
 * consecutive data blocks. The start of each chirp can be calculated by the
 * following formula.
 * \code
 * frame_start = &sample_data[CHIRP_NUMBER *
                              num_rx_antennas *
                              num_samples_per_chirp *
 *                            ((data_format == RADAR_RX_DATA_REAL)? 1 : 2)];
 * \endcode
 *
 * The index calculation of a certain data sample captured from a certain RX
 * antenna depends on the data interleaving and on real/complex data format.
 * The following code examples demonstrate how to access a data sample.
 * \code
 * // interleaved_rx = 0, data_format = RADAR_RX_DATA_REAL
 * // --------------------------------------------------------------------
 * data_value = frame_start[ANTENNA_NUMBER * num_samples_per_chirp +
 *                          SAMPLE_NUMBER];
 *
 * // interleaved_rx = 0, data_format = RADAR_RX_DATA_COMPLEX
 * // --------------------------------------------------------------------
 * data_value_real = frame_start[(2 * ANTENNA_NUMBER    ) *
 *                               num_samples_per_chirp + SAMPLE_NUMBER];
 * data_value_imag = frame_start[(2 * ANTENNA_NUMBER + 1) *
 *                               num_samples_per_chirp + SAMPLE_NUMBER];
 *
 * // interleaved_rx = 0, data_format = RADAR_RX_DATA_COMPLEX_INTERLEAVED
 * // --------------------------------------------------------------------
 * data_value_real = frame_start[2 * ANTENNA_NUMBER * num_samples_per_chirp +
 *                               SAMPLE_NUMBER];
 * data_value_imag = frame_start[2 * ANTENNA_NUMBER * num_samples_per_chirp +
 *                               SAMPLE_NUMBER + 1];
 *
 * // interleaved_rx = 1, data_format = RADAR_RX_DATA_REAL
 * // --------------------------------------------------------------------
 * data_value = pFrameStart[SAMPLE_NUMBER * num_rx_antennas + ANTENNA_NUMBER];
 *
 * // interleaved_rx = 1, data_format = RADAR_RX_DATA_COMPLEX
 * // --------------------------------------------------------------------
 * data_value_real = frame_start[SAMPLE_NUMBER * num_rx_antennas +
 *                               ANTENNA_NUMBER];
 * data_value_imag = frame_start[(num_samples_per_chirp + SAMPLE_NUMBER) *
 *                               num_rx_antennas + ANTENNA_NUMBER];
 *
 * // interleaved_rx = 1, data_format = RADAR_RX_DATA_COMPLEX_INTERLEAVED
 * // --------------------------------------------------------------------
 * data_value_real = frame_start[2 * SAMPLE_NUMBER * num_rx_antennas +
 *                               ANTENNA_NUMBER];
 * data_value_imag = frame_start[2 * SAMPLE_NUMBER * num_rx_antennas +
 *                               ANTENNA_NUMBER + 1];
 * \endcode
 *
 * In the code examples above, ANTENNA_NUMBER does not refer to the physical
 * antenna number, but the n-th enabled antenna. If for example rx_mask = 0xA,
 * ANTENNA_NUMBER=0 refers to antenna 0010b and ANTENNA_NUMBER=1 refers to
 * antenna 1000b. Furthermore it is assumed that pDataBuffer and frame_start
 * are casted to (uint8_t*) or (uint16_t*) according to ADC resolution.
 *
 * In total the data buffer contains num_chirps * num_rx_antennas *
 * num_samples_per_chirp values. If radar data is complex, that value is
 * multiplied by 2.
 */
typedef struct
{
    const float*     sample_data;            /**< The buffer containing the
                                                  radar data */
    uint32_t         frame_number;           /**< The running number of the
                                                  data frame. The frame
                                                  counter is, reset every time
                                                  \ref ep_radar_base_set_automatic_frame_trigger
                                                  is called. If automatic
                                                  frame trigger is not active,
                                                  the frame counter may not
                                                  work, and this could be 0.
                                                  */
    uint32_t         num_chirps;             /**< The number of chirps in this
                                                 frame. */
    uint8_t          num_rx_antennas;        /**< The number of RX signals
                                                  that have been acquired with
                                                  each chirp. */
    uint32_t         num_samples_per_chirp;  /**< The number of samples
                                                  acquired in each chirp for
                                                  each enabled RX antenna. */
    uint8_t          rx_mask;                /**< Each antenna is represented
                                                  by a bit in this mask. If
                                                  the bit is set, the
                                                  according RX antenna was
                                                  used to capture data in this
                                                  frame. */
    uint8_t          adc_resolution;         /**< The ADC resolution of the
                                                  data in sample_data. */
    uint8_t          interleaved_rx;         /**< If this is 0, the radar data
                                                  of multiple RX antennas is
                                                  stored in consecutive data
                                                  blocks, where each block
                                                  holds data of one antenna.
                                                  If this is non-zero, the
                                                  radar data of multiple RX
                                                  antennas is stored in one
                                                  data block, where for each
                                                  point in time the samples
                                                  from all RX antennas are
                                                  stored consecutively before
                                                  the data of the next point
                                                  in time follows. */
    Rx_Data_Format_t data_format;            /**< This indicates if the data
                                                  is sample_data is real or
                                                  complex, and if complex data
                                                  is interleaved. */
} Frame_Info_t;

/**
 * \brief This is the callback type for radar frame data.
 *
 * Whenever a sensor board sends a message containing radar frame data, a
 * callback of this type is issued. The user must register the callback
 * function by calling \ref ep_radar_base_set_callback_data_frame.
 *
 * Whenever a callback is issued, the following parameters are passed to the
 * callback:
 *
 * \param[in] context          The context data pointer, provided along with
 *                             the callback itself through
 *                             \ref ep_radar_base_set_callback_data_frame.
 * \param[in] protocol_handle  The handle of the connection, the sending
 *                             device is connected to.
 * \param[in] endpoint         The number of the endpoint that has sent the
 *                             message.
 * \param[in] frame_info       The radar frame data from the received message.
 */
typedef void(*Callback_Data_Frame_t)(void* context, int32_t protocol_handle,
                                     uint8_t endpoint,
                                     const Frame_Info_t* frame_info);

/**
 * \brief This is the callback type for driver version information.
 *
 * Whenever a sensor board sends a message containing driver version
 * information, a callback of this type is issued. The user must register the
 * callback function by calling
 * \ref ep_radar_base_set_callback_driver_version.
 *
 * Whenever a callback is issued, the following parameters are passed to the
 * callback:
 *
 * \param[in] context          The context data pointer, provided along with
 *                             the callback itself through
 *                             \ref ep_radar_base_set_callback_driver_version.
 * \param[in] protocol_handle  The handle of the connection, the sending
 *                             device is connected to.
 * \param[in] endpoint         The number of the endpoint that has sent the
 *                             message.
 * \param[in] driver_version   The driver version information from the
 *                             received message.
 */
typedef void(*Callback_Driver_Version_t)(void* context,
                                         int32_t protocol_handle,
                                         uint8_t endpoint,
                                         const Driver_Version_t*
                                           driver_version);

/**
 * \brief This is the callback type for device information.
 *
 * Whenever a sensor board sends a message containing device information, a
 * callback of this type is issued. The user must register the callback
 * function by calling \ref ep_radar_base_set_callback_device_info.
 *
 * Whenever a callback is issued, the following parameters are passed to the
 * callback:
 *
 * \param[in] context          The context data pointer, provided along with
 *                             the callback itself through
 *                             \ref ep_radar_base_set_callback_device_info.
 * \param[in] protocol_handle  The handle of the connection, the sending
 *                             device is connected to.
 * \param[in] endpoint         The number of the endpoint that has sent the
 *                             message.
 * \param[in] device_info      The device information from the received
 *                             message.
 */
typedef void(*Callback_Device_Info_t)(void* context, int32_t protocol_handle,
                                      uint8_t endpoint,
                                      const Device_Info_t* device_info);

/**
 * \brief This is the callback type for frame format.
 *
 * Whenever a sensor board sends a message containing the frame format, a
 * callback of this type is issued. The user must register the callback
 * function by calling \ref ep_radar_base_set_callback_frame_format.
 *
 * Whenever a callback is issued, the following parameters are passed to the
 * callback:
 *
 * \param[in] context          The context data pointer, provided along with
 *                             the callback itself through
 *                             \ref ep_radar_base_set_callback_frame_format.
 * \param[in] protocol_handle  The handle of the connection, the sending
 *                             device is connected to.
 * \param[in] endpoint         The number of the endpoint that has sent the
 *                             message.
 * \param[in] frame_format     The frame format from the received message.
 */
typedef void(*Callback_Frame_Format_t)(void* context, int32_t protocol_handle,
                                       uint8_t endpoint,
                                       const Frame_Format_t* frame_format);

/**
 * \brief This is the callback type for a temperature value.
 *
 * Whenever a sensor board sends a message containing a temperature value, a
 * callback of this type is issued. The user must register the callback
 * function by calling \ref ep_radar_base_set_callback_temperature.
 *
 * Whenever a callback is issued, the following parameters are passed to the
 * callback:
 *
 * \param[in] context           The context data pointer, provided along with
 *                              the callback itself through
 *                              \ref ep_radar_base_set_callback_temperature.
 * \param[in] protocol_handle   The handle of the connection, the sending
 *                              device is connected to.
 * \param[in] endpoint          The number of the endpoint that has sent the
 *                              message.
 * \param[in] temp_sensor       The number of the temperature sensor used for
 *                              the measurement.
 * \param[in] temperature_001C  The temperature value in 0.001 degree Celsius.
 */
typedef void(*Callback_Temperature_t)(void* context, int32_t protocol_handle,
                                      uint8_t endpoint, uint8_t temp_sensor,
                                      int32_t temperature_001C);

/**
 * \brief This is the callback type for a transmission power value.
 *
 * Whenever a sensor board sends a message containing a transmission power
 * value, a callback of this type is issued. The user must register the
 * callback function by calling \ref ep_radar_base_set_callback_tx_power.
 *
 * Whenever a callback is issued, the following parameters are passed to the
 * callback:
 *
 * \param[in] context          The context data pointer, provided along with
 *                             the callback itself through
 *                             \ref ep_radar_base_set_callback_tx_power.
 * \param[in] protocol_handle  The handle of the connection, the sending
 *                             device is connected to.
 * \param[in] endpoint         The number of the endpoint that has sent the
 *                             message.
 * \param[in] tx_antenna       The number of the TX antenna from which the
 *                             power was measured.
 * \param[in] tx_power_001dBm  The power value in 0.001 dBm.
 */
typedef void(*Callback_Tx_Power_t)(void* context, int32_t protocol_handle,
                                   uint8_t endpoint, uint8_t tx_antenna,
                                   int32_t tx_power_001dBm);

/**
 * \brief This is the callback type for a chirp duration value.
 *
 * Whenever a sensor board sends a message containing the chirp duration, a
 * callback of this type is issued. The user must register the callback
 * function by calling \ref ep_radar_base_set_callback_chirp_duration.
 *
 * Whenever a callback is issued, the following parameters are passed to the
 * callback:
 *
 * \param[in] context            The context data pointer, provided along with
 *                               the callback itself through
 *                               \ref ep_radar_base_set_callback_chirp_duration.
 * \param[in] protocol_handle    The handle of the connection, the sending
 *                               device is connected to.
 * \param[in] endpoint           The number of the endpoint that has sent the
 *                               message.
 * \param[in] chirp_duration_ns  The current chirp duration in nano seconds.
 */
typedef void(*Callback_Chirp_Duration_t)(void* context,
                                         int32_t protocol_handle,
                                         uint8_t endpoint,
                                         uint32_t chirp_duration_ns);

/**
 * \brief This is the callback type for the minimum frame interval value.
 *
 * Whenever a sensor board sends a message containing the minimum frame
 * interval, a callback of this type is issued. The user must register the
 * callback function by calling
 * \ref ep_radar_base_set_callback_min_frame_interval.
 *
 * Whenever a callback is issued, the following parameters are passed to the
 * callback:
 *
 * \param[in] context                The context data pointer, provided along
 *                                   with the callback itself through
 *                                   \ref ep_radar_base_set_callback_min_frame_interval.
 * \param[in] protocol_handle        The handle of the connection, the sending
 *                                   device is connected to.
 * \param[in] endpoint               The number of the endpoint that has sent
 *                                   the message.
 * \param[in] min_frame_interval_us  The current minimum frame interval in
 *                                   micro seconds.
 */
typedef void(*Callback_Min_Frame_Interval_t)(void* context,
                                             int32_t protocol_handle,
                                             uint8_t endpoint,
                                             uint32_t min_frame_interval_us);

/*
==============================================================================
   5. FUNCTION PROTOTYPES AND INLINE FUNCTIONS
==============================================================================
*/

/**
 * \brief This function checks if an endpoint in a device is an radar
 *        endpoint.
 *
 * This function checks type and version of the specified endpoint in a
 * connected device and returns a code that indicates if that endpoint is
 * compatible to the radar endpoint implementation in this module.
 * 
 * \param[in] protocol_handle  A handle to the connection to the sensor
 *                             device.
 * \param[in] endpoint         The endpoint in the connected device to be
 *                             checked for compatibility.
 *
 * \return If the specified endpoint is compatible to this implementation the
 *         function returns 0. If the endpoint is not compatible, a negative
 *         error code is returned.
 */
int32_t ep_radar_base_is_compatible_endpoint(int32_t protocol_handle,
                                             uint8_t endpoint);

/**
 * \brief This functions registers a callback for radar frame data messages.
 *
 * If a callback for this message type is registered, that callback is issued
 * every time, a connected board sends a message containing radar frame data.
 * If no callback is registered the message is ignored.
 *
 * Connection and handle and endpoint number of the sending endpoint are
 * passed to the callback along with the message data. Furthermore the pointer
 * context set by this function is forwarded to the callback.
 *
 * For more information about the callback function see
 * \ref Callback_Data_Frame_t.
 *
 * \param[in] callback  The function to be called when a radar frame data
 *                      message is received.
 * \param[in] context   A data pointer that is forwarded to the callback
 *                      function.
 */
void ep_radar_base_set_callback_data_frame(Callback_Data_Frame_t callback,
                                           void* context);

/**
 * \brief This functions registers a callback for driver version messages.
 *
 * If a callback for this message type is registered, that callback is issued
 * every time, a connected board sends a message containing driver version
 * information. If no callback is registered the message is ignored.
 *
 * Connection and handle and endpoint number of the sending endpoint are
 * passed to the callback along with the message data. Furthermore the pointer
 * context set by this function is forwarded to the callback.
 *
 * For more information about the callback function see
 * \ref Callback_Driver_Version_t.
 *
 * \param[in] callback  The function to be called when a radar frame data
 *                      message is received.
 * \param[in] context   A data pointer that is forwarded to the callback
 *                      function.
 */
void ep_radar_base_set_callback_driver_version(Callback_Driver_Version_t
                                                 callback,
                                               void* context);

/**
 * \brief This functions registers a callback for device info messages.
 *
 * If a callback for this message type is registered, that callback is issued
 * every time, a connected board sends a message containing device
 * information. If no callback is registered the message is ignored.
 *
 * Connection and handle and endpoint number of the sending endpoint are
 * passed to the callback along with the message data. Furthermore the pointer
 * context set by this function is forwarded to the callback.
 *
 * For more information about the callback function see
 * \ref Callback_Device_Info_t.
 *
 * \param[in] callback  The function to be called when a radar frame data
 *                      message is received.
 * \param[in] context   A data pointer that is forwarded to the callback
 *                      function.
 */
void ep_radar_base_set_callback_device_info(Callback_Device_Info_t callback,
                                            void* context);

/**
 * \brief This functions registers a callback for frame format messages.
 *
 * If a callback for this message type is registered, that callback is issued
 * every time, a connected board sends a message containing the frame format.
 * If no callback is registered the message is ignored.
 *
 * Connection and handle and endpoint number of the sending endpoint are
 * passed to the callback along with the message data. Furthermore the pointer
 * context set by this function is forwarded to the callback.
 *
 * For more information about the callback function see
 * \ref Callback_Frame_Format_t.
 *
 * \param[in] callback  The function to be called when a radar frame data
 *                      message is received.
 * \param[in] context   A data pointer that is forwarded to the callback
 *                      function.
 */
void ep_radar_base_set_callback_frame_format(Callback_Frame_Format_t callback,
                                             void* context);

/**
 * \brief This functions registers a callback for temperature messages.
 *
 * If a callback for this message type is registered, that callback is issued
 * every time, a connected board sends a message containing the frame format.
 * If no callback is registered the message is ignored.
 *
 * Connection and handle and endpoint number of the sending endpoint are
 * passed to the callback along with the message data. Furthermore the pointer
 * context set by this function is forwarded to the callback.
 *
 * For more information about the callback function see
 * \ref Callback_Temperature_t.
 *
 * \param[in] callback  The function to be called when a radar frame data
 *                      message is received.
 * \param[in] context   A data pointer that is forwarded to the callback
 *                      function.
 */
void ep_radar_base_set_callback_temperature(Callback_Temperature_t callback,
                                            void* context);

/**
 * \brief This functions registers a callback for transmission power messages.
 *
 * If a callback for this message type is registered, that callback is issued
 * every time, a connected board sends a message containing the frame format.
 * If no callback is registered the message is ignored.
 *
 * Connection and handle and endpoint number of the sending endpoint are
 * passed to the callback along with the message data. Furthermore the pointer
 * context set by this function is forwarded to the callback.
 *
 * For more information about the callback function see
 * \ref Callback_Tx_Power_t.
 *
 * \param[in] callback  The function to be called when a radar frame data
 *                      message is received.
 * \param[in] context   A data pointer that is forwarded to the callback
 *                      function.
 */
void ep_radar_base_set_callback_tx_power(Callback_Tx_Power_t callback,
                                         void* context);

/**
 * \brief This functions registers a callback for chirp duration messages.
 *
 * If a callback for this message type is registered, that callback is issued
 * every time, a connected board sends a message containing the frame format.
 * If no callback is registered the message is ignored.
 *
 * Connection and handle and endpoint number of the sending endpoint are
 * passed to the callback along with the message data. Furthermore the pointer
 * context set by this function is forwarded to the callback.
 *
 * For more information about the callback function see
 * \ref Callback_Chirp_Duration_t.
 *
 * \param[in] callback  The function to be called when a radar frame data
 *                      message is received.
 * \param[in] context   A data pointer that is forwarded to the callback
 *                      function.
 */
void ep_radar_base_set_callback_chirp_duration(Callback_Chirp_Duration_t
                                                 callback,
                                               void* context);

/**
 * \brief This functions registers a callback for frame interval messages.
 *
 * If a callback for this message type is registered, that callback is issued
 * every time, a connected board sends a message containing the frame format.
 * If no callback is registered the message is ignored.
 *
 * Connection and handle and endpoint number of the sending endpoint are
 * passed to the callback along with the message data. Furthermore the pointer
 * context set by this function is forwarded to the callback.
 *
 * For more information about the callback function see
 * \ref Callback_Min_Frame_Interval_t.
 *
 * \param[in] callback  The function to be called when a radar frame data
 *                      message is received.
 * \param[in] context   A data pointer that is forwarded to the callback
 *                      function.
 */
void ep_radar_base_set_callback_min_frame_interval(Callback_Min_Frame_Interval_t
                                                     callback,
                                                   void* context);

/**
 * \brief This function retrieves the next frame of radar data from the
 *        device.
 *
 * This function sends a message to the specified endpoint to retrieve radar
 * frame data. If the automatic frame trigger is active, this device will send
 * the next frame of radar data from internal FIFO memory. If no data is
 * available and bWait is non-zero, the device will wait until a new frame of
 * radar data becomes available. If bWait is zero, the device will respond
 * immediately with status code EP_RADAR_BASE_STATUS_TIME_OUT if no data is
 * available.
 *
 * If the automatic frame trigger is not active, this device triggers the
 * acquisition of a single frame and waits for the completion of the
 * acquisition. wait is ignored in this case.
 *
 * The function waits for the status code returned by the device and returns
 * it. If the device sends a message containing radar frame data a callback of
 * type \ref Callback_Data_Frame_t is issued.
 *
 * \param[in] protocol_handle  A handle to the connection to the sensor
 *                             device.
 * \param[in] endpoint         The endpoint in the connected device to send
 *                             the message to.
 * \param[in] wait             This flag indicates if the device will wait for
 *                             radar data or not.
 *
 * \return If a status message is received from the device the function
 *         returns a positive number containing the 16 bit status code in
 *         bits 0...15 and the endpoint that sent the code in bits 16...23.
 *         See \ref EndpointRadarStatusCodes for known status codes. The
 *         return value can be passed to
 *         \ref protocol_get_status_code_description to get a human readable
 *         description. If an error occurred a negative error code is
 *         returned.
 */
int32_t ep_radar_base_get_frame_data(int32_t protocol_handle,
                                     uint8_t endpoint, uint8_t wait);

/**
 * \brief This function starts or stops automatic radar frame trigger.
 *
 * This function sends a message to the specified endpoint to start or stop
 * the automatic frame trigger. If frame_interval_us is non-zero the automatic
 * trigger is started, if frame_interval_us is zero, the automatic trigger is
 * stopped.
 *
 * While the automatic frame trigger active, an internal timer in the device
 * triggers the acquisition of radar data frames with the given interval and
 * stores the acquired data in an internal FIFO memory. The data can be
 * retrieved from the FIFO memory by calling
 * \ref ep_radar_base_get_frame_data.
 *
 * The function waits for the status code returned by the device and returns
 * it.
 *
 * \param[in] protocol_handle    A handle to the connection to the sensor
 *                               device.
 * \param[in] endpoint           The endpoint in the connected device to send
 *                               the message to.
 * \param[in] frame_interval_us  The desired interval between the start of two
 *                               consecutive frame in micro seconds.
 *
 * \return If a status message is received from the device the function
 *         returns a positive number containing the 16 bit status code in
 *         bits 0...15 and the endpoint that sent the code in bits 16...23.
 *         See \ref EndpointRadarStatusCodes for known status codes. The
 *         return value can be passed to
 *         \ref protocol_get_status_code_description to get a human readable
 *         description. If an error occurred a negative error code is
 *         returned.
 */
int32_t ep_radar_base_set_automatic_frame_trigger(int32_t protocol_handle,
                                                  uint8_t endpoint,
                                                  uint32_t frame_interval_us);

/**
 * \brief This function enables a test mode for the antennas of the radar
 *        device.
 *
 * This function sends a message to the specified endpoint to enable the
 * antenna test mode. In that mode the device will emit a constant RF
 * frequency using the specified antenna configuration. The device stays in
 * this mode until this function is called again with tx_mask = 0 and
 * rx_mask = 0.
 *
 * While the device is in test mode, normal radar operation is not possible.
 * While automatic trigger is active, the test mode can not be activated.
 *
 * The function waits for the status code returned by the device and returns
 * it.
 *
 * \warning The device may get hot quickly in test mode. Overheating and
 *          damage of the radar device is possible.
 *
 * \param[in] protocol_handle  A handle to the connection to the sensor
 *                             device.
 * \param[in] endpoint         The endpoint in the connected device to send
 *                             the message to.
 * \param[in] tx_mask          A mask to enable the antennas for transmission.
 *                             Each bit of the mask represents an available TX
 *                             antenna. If a bit is set, the according antenna
 *                             is activated.
 * \param[in] rx_mask          A mask to enable RX antennas and demodulation
 *                             circuitry. Each bit of the mask represents an
 *                             available RX antenna. If a bit is set, the
 *                             according bit is activated.
 * \param[in] frequency_kHz    The frequency of the constant tone that is
 *                             transmitted. The value is specified in kHz. If
 *                             tx_antennaMask = 0, the specified frequency
 *                             will still be fed into the demodulation
 *                             circuitry of the RX chain.
 * \param[in] tx_power         The power value the constant tone is emitted
 *                             with. The value must be in the range of
 *                             0 ... max_tx_power (see also
 *                             \ref Device_Info_t). If tx_antennaMask = 0,
 *                             this value is ignored.
 *
 * \return If a status message is received from the device the function
 *         returns a positive number containing the 16 bit status code in
 *         bits 0...15 and the endpoint that sent the code in bits 16...23.
 *         See \ref EndpointRadarStatusCodes for known status codes. The
 *         return value can be passed to
 *         \ref protocol_get_status_code_description to get a human readable
 *         description. If an error occurred a negative error code is
 *         returned.
 */
int32_t ep_radar_base_enable_test_mode(int32_t protocol_handle,
                                       uint8_t endpoint, uint8_t tx_mask,
                                       uint8_t rx_mask,
                                       uint32_t frequency_kHz,
                                       uint8_t tx_power);

/**
 * \brief This function queries version information of the driver .
 *
 * This function sends a message to the specified endpoint to query version
 * information from the connected device. The function waits for the status
 * code returned by the device and returns it. The expected device info
 * message is forwarded through a callback of type
 * \ref Callback_Driver_Version_t.
 *
 * \param[in] protocol_handle  A handle to the connection to the sensor
 *                             device.
 * \param[in] endpoint         The endpoint in the connected device to send
 *                             the message to.
 *
 * \return If a status message is received from the device the function
 *         returns a positive number containing the 16 bit status code in
 *         bits 0...15 and the endpoint that sent the code in bits 16...23.
 *         See \ref EndpointRadarStatusCodes for known status codes. The
 *         return value can be passed to
 *         \ref protocol_get_status_code_description to get a human readable
 *         description. If an error occurred a negative error code is
 *         returned.
 */
int32_t ep_radar_base_get_driver_version(int32_t protocol_handle,
                                         uint8_t endpoint);

/**
 * \brief This function queries information about a device and its
 *        capabilities.
 *
 * This function sends a message to the specified endpoint to query device
 * information from the connected device. The function waits for the status
 * code returned by the device and returns it. The expected device info
 * message is forwarded through a callback of type
 * \ref Callback_Device_Info_t.
 *
 * \param[in] protocol_handle  A handle to the connection to the sensor
 *                             device.
 * \param[in] endpoint         The endpoint in the connected device to send
 *                             the message to.
 *
 * \return If a status message is received from the device the function
 *         returns a positive number containing the 16 bit status code in
 *         bits 0...15 and the endpoint that sent the code in bits 16...23.
 *         See \ref EndpointRadarStatusCodes for known status codes. The
 *         return value can be passed to
 *         \ref protocol_get_status_code_description to get a human readable
 *         description. If an error occurred a negative error code is
 *         returned.
 */
int32_t ep_radar_base_get_device_info(int32_t protocol_handle,
                                      uint8_t endpoint);

/**
 * \brief This function modifies the current radar data frame format.
 *
 * This function sends a message to the specified endpoint containing frame
 * format parameters to be applied. The connected device will change the radar
 * data format according to the provided structure of type
 * \ref Frame_Format_t.
 *
 * A value check is applied to each field of frame_format. If any value or the
 * combination of the values is out of range, the frame format is not changed
 * and an error is returned.
 *
 * The function waits for the status code returned by the device and returns
 * it. If the device sends any message containing parameter info in return,
 * those messages are forwarded to according callback functions.
 *
 * The configuration can not be changed while the automatic trigger is active.
 *
 * \param[in] protocol_handle  A handle to the connection to the sensor
 *                             device.
 * \param[in] endpoint         The endpoint in the connected device to send
 *                             the message to.
 * \param[in] frame_format     A pointer to the new frame configuration to be
 *                             applied.
 *
 * \return If a status message is received from the device the function
 *         returns a positive number containing the 16 bit status code in
 *         bits 0...15 and the endpoint that sent the code in bits 16...23.
 *         See \ref EndpointRadarStatusCodes for known status codes. The
 *         return value can be passed to
 *         \ref protocol_get_status_code_description to get a human readable
 *         description. If an error occurred a negative error code is
 *         returned.
 */
int32_t ep_radar_base_set_frame_format(int32_t protocol_handle,
                                       uint8_t endpoint,
                                       const Frame_Format_t* frame_format);

/**
 * \brief This function queries the current frame format.
 *
 * This function sends a message to the specified endpoint to query the
 * current frame format from the connected device. The function waits for the
 * status code returned by the device and returns it. The expected frame
 * format message is forwarded through a callback of type
 * \ref Callback_Frame_Format_t.
 *
 * \param[in] protocol_handle  A handle to the connection to the sensor
 *                             device.
 * \param[in] endpoint         The endpoint in the connected device to send
 *                             the message to.
 *
 * \return If a status message is received from the device the function
 *         returns a positive number containing the 16 bit status code in
 *         bits 0...15 and the endpoint that sent the code in bits 16...23.
 *         See \ref EndpointRadarStatusCodes for known status codes. The
 *         return value can be passed to
 *         \ref protocol_get_status_code_description to get a human readable
 *         description. If an error occurred a negative error code is
 *         returned.
 */
int32_t ep_radar_base_get_frame_format(int32_t protocol_handle,
                                       uint8_t endpoint);

/**
 * \brief This function queries the current temperature of the sensor device.
 *
 * This function sends a message to the specified endpoint to query the
 * temperature from the connected device. The function waits for the status
 * code returned by the device and returns it. The expected temperature
 * message is forwarded through a callback of type
 * \ref Callback_Temperature_t.
 *
 * A radar sensor device may contain more than one temperature sensors, so the
 * number of the sensor to be used for measuring must be specified. The number
 * of available temperature sensors is part of the \ref Device_Info_t
 * structure which can be queried by calling
 * \ref ep_radar_base_get_device_info. The number of the first temperature
 * sensor is 0.
 *
 * \param[in] protocol_handle  A handle to the connection to the sensor
 *                             device.
 * \param[in] endpoint         The endpoint in the connected device to send
 *                             the message to.
 * \param[in] temp_sensor      The number of the temperature sensor to be
 *                             used.
 *
 * \return If a status message is received from the device the function
 *         returns a positive number containing the 16 bit status code in
 *         bits 0...15 and the endpoint that sent the code in bits 16...23.
 *         See \ref EndpointRadarStatusCodes for known status codes. The
 *         return value can be passed to
 *         \ref protocol_get_status_code_description to get a human readable
 *         description. If an error occurred a negative error code is
 *         returned.
 */
int32_t ep_radar_base_get_temperature(int32_t protocol_handle,
                                      uint8_t endpoint, uint8_t temp_sensor);

/**
 * \brief This function queries the current RF transmission power from the
 *        sensor device.
 *
 * This function sends a message to the specified endpoint to query the
 * transmission power of the specified TX antenna from the connected device.
 * The function waits for the status code returned by the device and returns
 * it. The expected TX power message is forwarded through a callback of type
 * \ref Callback_Tx_Power_t.
 *
 * Typically a radar sensor device contains a power sensor for each
 * transmission antenna. The number of transmission antennas is part of the
 * \ref Device_Info_t structure which can be queried by calling
 * \ref ep_radar_base_get_device_info. The number of the first antenna (and TX
 * power sensor) is 0.
 *
 * \param[in] protocol_handle  A handle to the connection to the sensor
 *                             device.
 * \param[in] endpoint         The endpoint in the connected device to send
 *                             the message to.
 * \param[in] tx_antenna       The number of the TX antenna whose transmission
 *                             power is requested.
 *
 * \return If a status message is received from the device the function
 *         returns a positive number containing the 16 bit status code in
 *         bits 0...15 and the endpoint that sent the code in bits 16...23.
 *         See \ref EndpointRadarStatusCodes for known status codes. The
 *         return value can be passed to
 *         \ref protocol_get_status_code_description to get a human readable
 *         description. If an error occurred a negative error code is
 *         returned.
 */
int32_t ep_radar_base_get_tx_power(int32_t protocol_handle, uint8_t endpoint,
                                   uint8_t tx_antenna);

/**
 * \brief This function queries the duration for a single chirp from the
 *        sensor device.
 *
 * This function sends a message to the specified endpoint to query the
 * duration of a single chirp from the connected device. The function waits
 * for the status code returned by the device and returns it. The expected
 * chirp duration message is forwarded through a callback of type
 * \ref Callback_Chirp_Duration_t.
 *
 * \param[in] protocol_handle  A handle to the connection to the sensor
 *                             device.
 * \param[in] endpoint         The endpoint in the connected device to send
 *                             the message to.
 *
 * \return If a status message is received from the device the function
 *         returns a positive number containing the 16 bit status code in
 *         bits 0...15 and the endpoint that sent the code in bits 16...23.
 *         See \ref EndpointRadarStatusCodes for known status codes. The
 *         return value can be passed to
 *         \ref protocol_get_status_code_description to get a human readable
 *         description. If an error occurred a negative error code is
 *         returned.
 */
int32_t ep_radar_base_get_chirp_duration(int32_t protocol_handle,
                                         uint8_t endpoint);

/**
 * \brief This function queries the minimum frame interval from the sensor
 *        device.
 *
 * This function sends a message to the specified endpoint to query the
 * minimum frame interval from the connected device. The function waits for
 * the status code returned by the device and returns it. The expected message
 * containing the frame interval is forwarded through a callback of type
 * \ref Callback_Min_Frame_Interval_t.
 *
 * \param[in] protocol_handle  A handle to the connection to the sensor
 *                             device.
 * \param[in] endpoint         The endpoint in the connected device to send
 *                             the message to.
 *
 * \return If a status message is received from the device the function
 *         returns a positive number containing the 16 bit status code in
 *         bits 0...15 and the endpoint that sent the code in bits 16...23.
 *         See \ref EndpointRadarStatusCodes for known status codes. The
 *         return value can be passed to
 *         \ref protocol_get_status_code_description to get a human readable
 *         description. If an error occurred a negative error code is
 *         returned.
 */
int32_t ep_radar_base_get_min_frame_interval(int32_t protocol_handle,
                                             uint8_t endpoint);

/** @} */

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

/* End of include guard */
#endif /* ENDPOINTRADARBASE_H_INCLUDED */

/* --- End of File -------------------------------------------------------- */
