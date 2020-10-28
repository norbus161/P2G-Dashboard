/**
 * \file EndpointRadarDoppler.h
 *
 * \brief This file defines the API to communicate with Radar Doppler
 *        Endpoints in Infineon sensor devices.
 *
 * The module EndpointRadarDoppler.c provides functions to communicate with
 * Infineon radar sensor devices. The user must use the function provided by
 * the module \ref Protocol.c to open a connection to a radar sensor device
 * and query information about the endpoints in the device.
 *
 * The function \ref ep_radar_doppler_is_compatible_endpoint can be used to
 * check each endpoint if it is compatible with this API. If it is the types
 * and functions in from this module can be used to send parameters to the
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

#ifndef ENDPOINTRADARDOPPLER_H_INCLUDED
#define ENDPOINTRADARDOPPLER_H_INCLUDED

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
 * \defgroup EndpointRadarDoppler EndpointRadarDoppler
 *
* \brief EndpointRadarDoppler documentation.
 *
 * @{
 */

/*
==============================================================================
   3. TYPES
==============================================================================
*/

/**
 * \brief This structure holds the parameters to configure the RF behaviour in
 *        Doppler radar mode.
 *
 * A structure of this type must be passed to
 * \ref ep_radar_doppler_set_doppler_configuration to change the RF settings.
 * When the sensor board sends the current RF settings, a structure of this
 * type is returned through the callback
 * \ref Callback_Doppler_Configuration_t. Use
 * \ref ep_radar_doppler_get_doppler_configuration to query the current RF
 * settings.
 */
typedef struct
{
    uint32_t frequency_kHz;  /**< The lower frequency of an FMCW chirp. The
                                  value is specified in kHz. */
    uint8_t  tx_power;       /**< The transmission power the emitted FMCW
                                  chirps. The value is expected to be in the
                                  range of 0...max_tx_power (see also
                                  \ref Device_Info_t). */
} Doppler_Configuration_t;

/**
 * \brief This is the callback type for Doppler RF configuration.
 *
 * Whenever a sensor board sends a message containing Doppler RF
 * configuration, a callback of this type is issued. The user must register
 * the callback function by calling
 * \ref ep_radar_doppler_set_callback_doppler_configuration.
 *
 * Whenever a callback is issued, the following parameters are passed to the
 * callback:
 *
 * \param[in] context           The context data pointer, provided along with
 *                              the callback itself through
 *                              \ref ep_radar_doppler_set_callback_doppler_configuration.
 * \param[in] protocol_handle   The handle of the connection, the sending
 *                              device is connected to.
 * \param[in] endpoint          The number of the endpoint that has sent the
 *                              message.
 * \param[in] doppler_settings  The Doppler RF configuration from the received
 *                              message.
 */
typedef void(*Callback_Doppler_Configuration_t)(void* context,
                                                int32_t protocol_handle,
                                                uint8_t endpoint,
                                                const Doppler_Configuration_t*
                                                  doppler_settings);

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
int32_t ep_radar_doppler_is_compatible_endpoint(int32_t protocol_handle,
                                                uint8_t endpoint);

/**
 * \brief This functions registers a callback for Doppler RF configuration
 *        messages.
 *
 * If a callback for this message type is registered, that callback is issued
 * every time, a connected board sends a message containing Doppler RF
 * configuration. If no callback is registered the message is ignored.
 *
 * Connection and handle and endpoint number of the sending endpoint are
 * passed to the callback along with the message data. Furthermore the pointer
 * context set by this function is forwarded to the callback.
 *
 * For more information about the callback function see
 * \ref Callback_Doppler_Configuration_t.
 *
 * \param[in] callback  The function to be called when a radar frame data
 *                      message is received.
 * \param[in] context   A data pointer that is forwarded to the callback
 *                      function.
 */
void ep_radar_doppler_set_callback_doppler_configuration(Callback_Doppler_Configuration_t
                                                           callback,
                                                         void* context);

/**
 * \brief This function modifies the Doppler RF parameters.
 *
 * This function sends a message to the specified endpoint containing Doppler
 * RF parameters to be applied. The connected device function will change the
 * RF parameters according to the provided
 * structure of type \ref Doppler_Configuration_t.
 *
 * A value check is applied to each field of pRFConfiguration. If any value or
 * the combination of the values is out of range, the Doppler RF parameters
 * are not changed and an error is returned.
 *
 * The function waits for the status code returned by the device and returns
 * it. If the device sends any message containing parameter info in return, 
 * those messages are forwarded to according callback functions.
 *
 * The configuration can not be changed while the automatic trigger is active.
 *
 * \param[in] protocol_handle        A handle to the connection to the sensor
 *                                   device.
 * \param[in] endpoint               The endpoint in the connected device to
 *                                   send the message to.
 * \param[in] doppler_configuration  A pointer to the new Doppler RF
 *                                   parameters to be applied.
 *
 * \return If a status message is received from the device the function
 *         returns a positive number containing the 16 bit status code in
 *         bits 0...15 and the endpoint that sent the code in bits 16...23.
 *         See \ref EndpointRadarStatusCodes for known status codes. The
 *         return can be passed to \ref protocol_get_status_code_description
 *         to get a human readable value description. If an error occurred a
 *         negative error code is returned.
 */
int32_t ep_radar_doppler_set_doppler_configuration(int32_t protocol_handle,
                                                   uint8_t endpoint,
                                                   const
                                                    Doppler_Configuration_t*
                                                     doppler_configuration);

/**
 * \brief This function queries the current Doppler RF parameters.
 *
 * This function sends a message to the specified endpoint to query the
 * current Doppler RF parameters from the connected device. The function waits
 * for the status code returned by the device and returns it. The expected RF
 * parameter message is forwarded through a callback of type
 * \ref Callback_Doppler_Configuration_t.
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
 *         return can be passed to \ref protocol_get_status_code_description
 *         to get a human readable value description. If an error occurred a
 *         negative error code is returned.
 */
int32_t ep_radar_doppler_get_doppler_configuration(int32_t protocol_handle,
                                                   uint8_t endpoint);

/** @} */

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

/* End of include guard */
#endif /* ENDPOINTRADARDOPPLER_H_INCLUDED */

/* --- End of File -------------------------------------------------------- */
