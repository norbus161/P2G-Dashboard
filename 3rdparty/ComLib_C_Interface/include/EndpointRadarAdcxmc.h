/**
 * \file EndpointRadarADCXMC.h
 *
 * \brief This file defines the API to communicate with Radar ADC XMC
 *        Endpoints in Infineon sensor devices.
 *
 * The module EndpointRadarADCXMC.c provides functions to communicate with
 * Infineon radar sensor devices. The user must use the function provided by
 * the module \ref Protocol.c to open a connection to a radar sensor device
 * and query information about the endpoints in the device.
 *
 * The function \ref ep_radar_adcxmc_is_compatible_endpoint can be used to check
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

#ifndef ENDPOINTRADARADCXMC_H_INCLUDED
#define ENDPOINTRADARADCXMC_H_INCLUDED

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
 * \defgroup EndpointRadarADCXMC EndpointRadarADCXMC
 *
 * \brief EndpointRadarADCXMC documentation.
 *
 * @{
 */

/*
==============================================================================
   3. TYPES
==============================================================================
*/

/**
 * \brief This structure holds the parameters to configure the Analog/Digital
 *        Converter.
 *
 * A struct of this type must be passed to
 * \ref ep_radar_adcxmc_set_adc_configuration to change the ADC settings. When
 * the sensor board sends the current ADC settings, a structure of this type
 * is returned through the callback \ref Callback_Adc_Xmc_Configuration_t. Use
 * \ref ep_radar_adcxmc_get_adc_configuration to query the current ADC
 * settings.
 */
typedef struct
{
    uint32_t samplerate_Hz;        /**< The sampling rate at with the IF
                                        signals are captured. The value is
                                        specified in Hz.  This value affects
                                        the chirp duration. */
    uint8_t resolution;            /**< The number of bits for each signal
                                        sample. The value may affect the
                                        highest possible sampling rate */
    uint8_t use_post_calibration;  /**< If this is non-zero the post
                                        calibration of the ADC is used. */
} Adc_Xmc_Configuration_t;

/**
 * \brief This is the callback type for ADCconfiguration.
 *
 * Whenever a sensor board sends a message containing ADC configuration, a
 * callback of this type is issued. The user must register the callback
 * function by calling \ref ep_radar_adcxmc_set_callback_adc_configuration.
 *
 * Whenever a callback is issued, the following parameters are passed to the
 * callback:
 *
 * \param[in] context            The context data pointer, provided along with
 *                               the callback itself through
 *                               \ref ep_radar_adcxmc_set_callback_adc_configuration.
 * \param[in] protocol_handle    The handle of the connection, the sending
 *                               device is connected to.
 * \param[in] endpoint           The number of the endpoint that has sent the
 *                               message.
 * \param[in] adc_configuration  The ADC configuration from the received
 *                               message.
 */
typedef void(*Callback_Adc_Xmc_Configuration_t)(void* context,
                                                int32_t protocol_handle,
                                                uint8_t endpoint,
                                                const Adc_Xmc_Configuration_t*
                                                  adc_configuration);

/*
==============================================================================
   5. FUNCTION PROTOTYPES AND INLINE FUNCTIONS
==============================================================================
*/

/**
 * \brief This function checks if an endpoint in a device is an radar Doppler
 *        endpoint.
 *
 * This function checks type and version of the specified endpoint in a
 * connected device and returns a code that indicates if that endpoint is
 * compatible to the radar FMCW endpoint implementation in this module.
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
int32_t ep_radar_adcxmc_is_compatible_endpoint(int32_t protocol_handle,
                                               uint8_t endpoint);

/**
 * \brief This functions registers a callback for ADC configuration messages.
 *
 * If a callback for this message type is registered, that callback is issued
 * every time, a connected board sends a message containing ADC configuration.
 * If no callback is registered the message is ignored.
 *
 * Connection and handle and endpoint number of the sending endpoint are
 * passed to the callback along with the message data. Furthermore the pointer
 * context set by this function is forwarded to the callback.
 *
 * For more information about the callback function see
 * \ref Callback_Adc_Xmc_Configuration_t.
 *
 * \param[in] callback  The function to be called when a radar frame data
 *                      message is received.
 * \param[in] context   A data pointer that is forwarded to the callback
 *                      function.
 */
void ep_radar_adcxmc_set_callback_adc_configuration(Callback_Adc_Xmc_Configuration_t
                                                      callback,
                                                    void* context);

/**
 * \brief This function modifies the current ADC configuration.
 *
 * This function sends a message to the specified endpoint containing ADC
 * parameters to be applied. The connected device function will change the ADC
 * parameters according to the provided structure of type
 * \ref Adc_Xmc_Configuration_t.
 *
 * A value check is applied to each field of adc_configuration. If any value
 * or the combination of the values is out of range, the ADC parameters are
 * not changed and an error is returned.
 *
 * If the field samplerate_Hz in adc_configuration is 0 the sampling rate will
 * be set to the highest possible value.
 *
 * The function waits for the status code returned by the device and returns
 * it. If the device sends any message containing parameter info in return,
 * those messages are forwarded to according callback functions.
 *
 * The configuration can not be changed while the automatic trigger is active.
 *
 * \param[in] protocol_handle    A handle to the connection to the sensor
 *                               device.
 * \param[in] endpoint           The endpoint in the connected device to send
 *                               the message to.
 * \param[in] adc_configuration  A pointer to the new ADC parameters to be
 *                               applied.
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
int32_t ep_radar_adcxmc_set_adc_configuration(int32_t protocol_handle,
                                              uint8_t endpoint,
                                              const Adc_Xmc_Configuration_t*
                                                adc_configuration);

/**
 * \brief This function queries the current ADC parameters.
 *
 * This function sends a message to the specified endpoint to query the
 * current ADC parameters from the connected device. The function waits for
 * the status code returned by the device and returns it. The expected ADC
 * parameter message is forwarded through a callback of type
 * \ref Callback_Adc_Xmc_Configuration_t.
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
int32_t ep_radar_adcxmc_get_adc_configuration(int32_t protocol_handle,
                                              uint8_t endpoint);

/** @} */

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

/* End of include guard */
#endif /* ENDPOINTRADARADCXMC_H_INCLUDED */

/* --- End of File -------------------------------------------------------- */
