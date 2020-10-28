/**
 * \file EndpointRadarFMCW.h
 *
 * \brief This file defines the API to communicate with Radar FMCW Endpoints
 *        in Infineon sensor devices.
 *
 * The module EndpointRadarFMCW.c provides functions to communicate with
 * Infineon radar sensor devices. The user must use the function provided by
 * the module \ref Protocol.c to open a connection to a radar sensor device
 * and query information about the endpoints in the device.
 *
 * The function \ref ep_radar_fmcw_is_compatible_endpoint can be used to check
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

#ifndef ENDPOINTRADARFMCW_H_INCLUDED
#define ENDPOINTRADARFMCW_H_INCLUDED

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
 * \defgroup EndpointRadarFMCW EndpointRadarFMCW
 *
* \brief EndpointRadarFMCW documentation.
 *
 * @{
 */

/*
==============================================================================
   3. TYPES
==============================================================================
*/

/**
 * \brief This enumeration type lists all possible chirp directions in FMCW
 *        radar mode.
 *
 * A value of this type is part of the struct \ref Fmcw_Configuration_t.
 *
 * To use the alternating modes the number of chirp per frame must be even,
 * otherwise the sensor board will refuse to switch to this mode.
 */
typedef enum
{
    EP_RADAR_FMCW_DIR_UPCHIRP_ONLY           = 0, /**< All chirps of a frame
                                                       start at the lower FMCW
                                                       frequency and ramp up
                                                       to the upper FMCW
                                                       frequency. */
    EP_RADAR_FMCW_DIR_DOWNCHIRP_ONLY         = 1, /**< All chirps of a frame
                                                       start at the upper FMCW
                                                       frequency and ramp down
                                                       to the lower FMCW
                                                       frequency. */
    EP_RADAR_FMCW_DIR_ALTERNATING_FIRST_UP   = 2, /**< Only the odd chirps of
                                                       a frame start at the
                                                       lower RF frequency and
                                                       ramp up to the upper
                                                       FMCW frequency. The
                                                       even chirps of a frame
                                                       go from the upper to
                                                       the lower FMCW
                                                       frequency. */
    EP_RADAR_FMCW_DIR_ALTERNATING_FIRST_DOWN = 3  /**< Only the even chirps of
                                                       a frame start at the
                                                       lower RF frequency and
                                                       ramp up to the upper
                                                       FMCW frequency. The odd
                                                       chirps of a frame go
                                                       from the upper to the
                                                       lower RF frequency. */
} Chirp_Direction_t;

/**
 * \brief This structure holds the parameters to configure the RF behaviour in
 *        FMCW radar mode.
 *
 * A structure of this type must be passed to
 * \ref ep_radar_fmcw_set_fmcw_configuration to change the FMCW settings. When
 * the sensor board sends the current FMCW settings, a structure of this type
 * is returned through the callback \ref Callback_Fmcw_Configuration_t. Use
 * \ref ep_radar_fmcw_get_fmcw_configuration to query the current FMCW
 * settings.
 */
typedef struct
{
    uint32_t          lower_frequency_kHz;  /**< The lower frequency of an
                                                 FMCW chirp. The value is
                                                 specified in kHz. */
    uint32_t          upper_frequency_kHz;  /**< The upper frequency of an
                                                 FMCW chirp. The value is
                                                 specified in kHz. */
    Chirp_Direction_t direction;            /**< The direction of the
                                                 frequency ramp during FMCW
                                                 operation. */
    uint8_t           tx_power;             /**< The transmission power the
                                                 emitted FMCW chirps. The
                                                 value is expected to be in
                                                 the range of 0...max_tx_power
                                                 (see also \ref Device_Info_t).
                                                 */
} Fmcw_Configuration_t;

/**
 * \brief This is the callback type for FMCW configuration.
 *
 * Whenever a sensor board sends a message containing FMCW configuration, a
 * callback of this type is issued. The user must register the callback
 * function by calling \ref ep_radar_fmcw_set_callback_fmcw_configuration.
 *
 * Whenever a callback is issued, the following parameters are passed to the
 * callback:
 *
 * \param[in] context             The context data pointer, provided along
 *                                with the callback itself through
 *                                \ref ep_radar_fmcw_set_callback_fmcw_configuration.
 * \param[in] protocol_handle     The handle of the connection, the sending
 *                                device is connected to.
 * \param[in] endpoint            The number of the endpoint that has sent the
 *                                message.
 * \param[in] fmcw_configuration  The FMCW configuration from the received
 *                                message.
 */
typedef void(*Callback_Fmcw_Configuration_t)(void* context,
                                             int32_t protocol_handle,
                                             uint8_t endpoint,
                                             const Fmcw_Configuration_t* 
                                               fmcw_configuration);

/**
 * \brief This is the callback type for the bandwidth per second value.
 *
 * Whenever a sensor board sends a message containing a bandwidth-per-second
 * value, a callback of this type is issued. The user must register the
 * callback function by calling
 * \ref ep_radar_fmcw_set_callback_bandwidth_per_second.
 *
 * Whenever a callback is issued, the following parameters are passed to the
 * callback:
 *
 * \param[in] context               The context data pointer, provided along
 *                                  with the callback itself through
 *                                  \ref ep_radar_fmcw_set_callback_bandwidth_per_second.
 * \param[in] protocol_handle       The handle of the connection, the sending
 *                                  device is connected to.
 * \param[in] endpoint              The number of the endpoint that has sent
 *                                  the message.
 * \param[in] bandwidth_per_second  The current bandwidth per second in frame
 *                                  interval in MHz/s.
 */
typedef void(*Callback_Bandwidth_Per_Second_t)(void* context,
                                               int32_t protocol_handle,
                                               uint8_t endpoint,
                                               uint32_t bandwidth_per_second);

/*
==============================================================================
   5. FUNCTION PROTOTYPES AND INLINE FUNCTIONS
==============================================================================
*/

/**
 * \brief This function checks if an endpoint in a device is an radar FMCW
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
int32_t ep_radar_fmcw_is_compatible_endpoint(int32_t protocol_handle,
                                             uint8_t endpoint);

/**
 * \brief This functions registers a callback for FMCW configuration messages.
 *
 * If a callback for this message type is registered, that callback is issued
 * every time, a connected board sends a message containing FMCW
 * configuration. If no callback is registered the message is ignored.
 *
 * Connection and handle and endpoint number of the sending endpoint are
 * passed to the callback along with the message data. Furthermore the pointer
 * context set by this function is forwarded to the callback.
 *
 * For more information about the callback function see
 * \ref Callback_Fmcw_Configuration_t.
 *
 * \param[in] callback  The function to be called when a radar frame data
 *                      message is received.
 * \param[in] context   A data pointer that is forwarded to the callback
 *                      function.
 */
void ep_radar_fmcw_set_callback_fmcw_configuration(Callback_Fmcw_Configuration_t
                                                     callback,
                                                   void* context);

/**
 * \brief This functions registers a callback for bandwidth per second
 *        messages.
 *
 * If a callback for this message type is registered, that callback is issued
 * every time, a connected board sends a message containing the bandwidth per
 * second value. If no callback is registered the message is ignored.
 *
 * Connection and handle and endpoint number of the sending endpoint are
 * passed to the callback along with the message data. Furthermore the pointer
 * context set by this function is forwarded to the callback.
 *
 * For more information about the callback function see
 * \ref Callback_Bandwidth_Per_Second_t.
 *
 * \param[in] callback  The function to be called when a radar frame data
 *                      message is received.
 * \param[in] context   A data pointer that is forwarded to the callback
 *                      function.
 */
void ep_radar_fmcw_set_callback_bandwidth_per_second(Callback_Bandwidth_Per_Second_t
                                                       callback,
                                                     void* context);

/**
 * \brief This function modifies the FMCW RF parameters.
 *
 * This function sends a message to the specified endpoint containing FMCW RF
 * parameters to be applied. The connected device function will change the RF
 * parameters according to the provided structure of type
 * \ref Fmcw_Configuration_t.
 *
 * A value check is applied to each field of pRFConfiguration. If any value or
 * the combination of the values is out of range, the FMCW parameters are not
 * changed and an error is returned.
 *
 * The function waits for the status code returned by the device and returns
 * it. If the device sends any message containing parameter info in return,
 * those messages are forwarded to according callback functions.
 *
 * The configuration can not be changed while the automatic trigger is active.
 *
 * \param[in] protocol_handle     A handle to the connection to the sensor
 *                                device.
 * \param[in] endpoint            The endpoint in the connected device to send
 *                                the message to.
 * \param[in] fmcw_configuration  A pointer to the new FMCW RF parameters to
 *                                be applied.
 *
 * \return If a status message is received from the device the function
 *         returns a positive number containing the 16 bit status code in
 *         bits 0...15 and the endpoint that sent the code in bits 16...23.
 *         See \ref EndpointRadarStatusCodes for known status codes. The
 *         return can be passed to \ref protocol_get_status_code_description
 *         to get a human readable value description. If an error occurred a
 *         negative error code is returned.
 */
int32_t ep_radar_fmcw_set_fmcw_configuration(int32_t protocol_handle,
                                             uint8_t endpoint,
                                             const Fmcw_Configuration_t*
                                               fmcw_configuration);

/**
 * \brief This function queries the current FMCW RF parameters.
 *
 * This function sends a message to the specified endpoint to query the
 * current FMCW RF parameters from the connected device. The function waits
 * for the status code returned by the device and returns it. The expected RF
 * parameter message is forwarded through a callback of type
 * \ref Callback_Fmcw_Configuration_t.
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
int32_t ep_radar_fmcw_get_fmcw_configuration(int32_t protocol_handle,
                                             uint8_t endpoint);

/**
 * \brief This function queries the current bandwidth per second.
 *
 * This function sends a message to the specified endpoint to query the
 * current bandwidth per second from the connected device. The function waits
 * for the status code returned by the device and returns it. The expected
 * timing information message is forwarded through a callback of type
 * \ref Callback_Bandwidth_Per_Second_t.
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
int32_t ep_radar_fmcw_get_bandwidth_per_second(int32_t protocol_handle,
                                             uint8_t endpoint);

/** @} */

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

/* End of include guard */
#endif /* ENDPOINTRADARFMCW_H_INCLUDED */

/* --- End of File -------------------------------------------------------- */
