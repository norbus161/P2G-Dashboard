/**
 * \file EndpointRadarIndustrial.h
 *
 * \brief This file contains the API methods applicable generically to Industrial radar demo kits, e.g. Distance2Go, Position2Go etc.
 *
 */
 
/* ===========================================================================
** Copyright (C) 2018-2019 Infineon Technologies AG
** All rights reserved.
** ===========================================================================
**
** ===========================================================================
** This document contains proprietary information of Infineon Technologies AG.
** Passing on and copying of this document, and communication of its contents
** is not permitted without Infineon's prior written authorisation.
** ===========================================================================
*/

#ifndef ENDPOINTRADARINDUSTRIAL_H_INCLUDED
#define ENDPOINTRADARINDUSTRIAL_H_INCLUDED

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
 * \defgroup EndpointRadarIndustrial EndpointRadarIndustrial
 *
 * \brief EndpointRadarIndustrial documentation.
 *
 * @{
 */				   

/*
==============================================================================
   5. FUNCTION PROTOTYPES AND INLINE FUNCTIONS
==============================================================================
*/

/**
* \brief This function checks if an endpoint in a device is a Radar Industrial
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
int32_t ep_radar_industrial_is_compatible_endpoint(int32_t protocol_handle,
												   uint8_t endpoint);

/**
* \brief This is the callback type for reading the status of Duty Cycle isenable().
*
* Whenever a sensor board query the status of duty cycle is enable, a
* callback of this type is issued. The user must register the callback
* function by calling \ref ep_radar_industrial_set_callback_duty_cycle_status.
*
* Whenever a callback is issued, the following parameters are passed to the
* callback:
*
* \param[in] context           The context data pointer, provided along with
*                              the callback itself through
*                              \ref ep_radar_industrial_set_callback_bgt_lna_status.
* \param[in] protocol_handle   The handle of the connection, the sending
*                              device is connected to.
* \param[in] endpoint          The number of the endpoint that has sent the
*                              message.
* \param[in] duty_cycle_is_enable     True (non-zero value) if Duty cycle gain is enabled, otherwise false (0 value).
*
*/
typedef void(*Callback_duty_cycle_status_t)(void* context, int32_t protocol_handle,
											uint8_t endpoint, uint8_t duty_cycle_is_enable);

/**
* \brief This is the callback type for reading the status of BGTLNA gain isenable().
*
* Whenever a sensor board query the status of BGT LNA gain is enable, a
* callback of this type is issued. The user must register the callback
* function by calling \ref ep_radar_industrial_set_callback_bgt_lna_status.
*
* Whenever a callback is issued, the following parameters are passed to the
* callback:
*
* \param[in] context           The context data pointer, provided along with
*                              the callback itself through
*                              \ref ep_radar_industrial_set_callback_bgt_lna_status.
* \param[in] protocol_handle   The handle of the connection, the sending
*                              device is connected to.
* \param[in] endpoint          The number of the endpoint that has sent the
*                              message.
* \param[in] pga_level_val     True (non-zero value) if BGT LNA gain is enabled, otherwise false (0 value).
*
*/
typedef void(*Callback_bgt_lna_status_t)(void* context, int32_t protocol_handle,
										 uint8_t endpoint, uint8_t bgt_lna_is_enable);
										 
/**
* \brief This functions registers a callback for radar Industrial radar Duty Cycle status messages.
*
* If a callback for this message type is registered, that callback is issued
* every time, a connected board sends a message containing Industrial radar BGT LNA status.
* If no callback is registered the message is ignored.
*
* Connection and handle and endpoint number of the sending endpoint are
* passed to the callback along with the message data. Furthermore the pointer
* context set by this function is forwarded to the callback.
*
* \param[in] callback  The function to be called when a radar frame data
*                      message is received.
* \param[in] context   A data pointer that is forwarded to the callback
*                      function.
*/
void ep_radar_industrial_set_callback_duty_cycle_status(Callback_duty_cycle_status_t callback, void* context);

/**
* \brief This functions registers a callback for radar Industrial radar BGT LNA status messages.
*
* If a callback for this message type is registered, that callback is issued
* every time, a connected board sends a message containing Industrial radar BGT LNA status.
* If no callback is registered the message is ignored.
*
* Connection and handle and endpoint number of the sending endpoint are
* passed to the callback along with the message data. Furthermore the pointer
* context set by this function is forwarded to the callback.
*
* \param[in] callback  The function to be called when a radar frame data
*                      message is received.
* \param[in] context   A data pointer that is forwarded to the callback
*                      function.
*/
void ep_radar_industrial_set_callback_bgt_lna_status(Callback_bgt_lna_status_t callback, void* context);

/**
* \brief This function checks the enable status of the Duty cycle, within the Industrial Radar demo kits
*
* By default, firmware has always enabled the duty cycle.
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
int32_t ep_radar_industrial_duty_cycle_is_enable(int32_t protocol_handle,
												 uint8_t endpoint);
											  
/**
* \brief This function enables the duty cycling mode within the Industrial Radar demo kits
*
* At the moment duty cycling is fixed and has no control over configuring the duty cycle percentage.
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
int32_t ep_radar_industrial_duty_cycle_enable(int32_t protocol_handle,
                                              uint8_t endpoint);
/**
* \brief This function disables the duty cycling mode within the Industrial Radar demo kits
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
int32_t ep_radar_industrial_duty_cycle_disable(int32_t protocol_handle,
                                               uint8_t endpoint);											 
/**
* \brief This function checks the enable status of the BGT LNA gain within the Industrial Radar demo kits
*
* By default, firmware has always enabled the BGT LNA gain.
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
int32_t ep_radar_industrial_bgt_lna_is_enable(int32_t protocol_handle,
                                              uint8_t endpoint);

/**
* \brief This function enables the receiver LNA gain of the BGTMTR1x within the Industrial Radar demo kits
*
* By default, firmware has always enabled the LNA gain.
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
int32_t ep_radar_industrial_bgt_lna_enable(int32_t protocol_handle,
										   uint8_t endpoint);

/**
* \brief This function disables the receiver LNA gain of the BGTMTR1x within the Industrial Radar demo kits
*
* In order to reduce the RX signal level to avoid ADC clipping, one can disable the BGT LNA gain via SPI commands.
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
int32_t ep_radar_industrial_bgt_lna_disable(int32_t protocol_handle,
                                            uint8_t endpoint);	

/** @} */

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

/* End of include guard */
#endif /* ENDPOINTRADARINDUSTRIAL_H_INCLUDED */

/* --- End of File -------------------------------------------------------- */
