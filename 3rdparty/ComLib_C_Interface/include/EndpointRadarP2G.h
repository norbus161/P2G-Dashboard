/**
 * \file EndpointRadarP2G.h
 *
 * \brief This file defines the API to communicate with Radar P2G Endpoints
 *        in Infineon Position2Go board.
 *
 * The module EndpointRadarP2G.c provides functions to communicate with
 * the P2G endpoint in Infineon Position2Go board. The user must 
 * use the function provided by the module \ref Protocol.c to open a connection 
 * to a radar sensor device and query information about the endpoints in the device.
 *
 * The function \ref ep_radar_p2g_is_compatible_endpoint can be used to check
 * each endpoint if it is compatible with this API. If it is the types and
 * functions in from this module can be used to send parameters to the
 * endpoint and query the current DSP settings and radar target information.
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

#ifndef ENDPOINTRADARP2G_H_INCLUDED
#define ENDPOINTRADARP2G_H_INCLUDED

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
 * \defgroup EndpointRadarP2G EndpointRadarP2G
 *
 * \brief EndpointRadarP2G documentation.
 *
 * @{
 */

/*
==============================================================================
  2. LOCAL DEFINITIONS
==============================================================================
*/


/**
 *  Radar P2G endpoint supports the following error codes.
 */
#define EP_RADAR_P2G_ERR_UNSUPPORTED_PGA_GAIN	0x0052	/**< The specified gain adjustment setting is not defined. */

/*
==============================================================================
   5. FUNCTION PROTOTYPES AND INLINE FUNCTIONS
==============================================================================
*/

/**
* \brief This function checks if an endpoint in a device is a Radar Position2Go (P2G)
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
int32_t ep_radar_p2g_is_compatible_endpoint(int32_t protocol_handle,
											uint8_t endpoint);
	
/**
 * \brief This is the callback type for reading the Programmable Gain Amplifier (PGA) level.
 *
 * Whenever a sensor board sends a message containing a PGA gain level value, a
 * callback of this type is issued. The user must register the callback
 * function by calling \ref ep_radar_p2g_set_callback_pga_level.
 *
 * Whenever a callback is issued, the following parameters are passed to the
 * callback:
 *
 * \param[in] context           The context data pointer, provided along with
 *                              the callback itself through
 *                              \ref ep_radar_p2g_set_callback_pga_level.
 * \param[in] protocol_handle   The handle of the connection, the sending
 *                              device is connected to.
 * \param[in] endpoint          The number of the endpoint that has sent the
 *                              message.
 * \param[in] pga_level_val     The gain level [0 - 7] of PGA used currently.
 *
 */
typedef void(*Callback_PGA_Level_t)(void* context, int32_t protocol_handle,
                                    uint8_t endpoint, uint16_t pga_level_val);

/**
* \brief This functions registers a callback for radar Position2Go data messages.
*
* If a callback for this message type is registered, that callback is issued
* every time, a connected board sends a message containing radar Position2Go data.
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
void ep_radar_p2g_set_callback_pga_level(Callback_PGA_Level_t callback, void* context);

/**
* \brief This function sets the programmable gain amplifier gain value for Position2Go demo kit.
*
* There are eight possible gain levels, one of them can be set by passing gain level argument [0-7] range.
*
* \param[in] protocol_handle  A handle to the connection to the sensor
*                             device.
* \param[in] endpoint         The endpoint in the connected device to be
*                             checked for compatibility.
* \param[in] pga_level        The PGA level to be set must be within the range [0 - 7].
*
* \return If the specified endpoint is compatible to this implementation the
*         function returns 0. If the endpoint is not compatible, a negative
*         error code is returned.
*/
int32_t ep_radar_p2g_set_pga_level(int32_t protocol_handle,
										  uint8_t endpoint,
										  uint16_t pga_level);
										  
/**
* \brief This function reads out the programmable gain amplifier gain value, currently used in Position2Go demo kit.
*
* There are eight possible gain levels, within [0-7] range. Each value corresponds to a binary gain value, please 
* refer to the PGA112 data-sheet.
*
* \param[in] protocol_handle  A handle to the connection to the sensor
*                             device.
* \param[in] endpoint         The endpoint in the connected device to be
*                             checked for compatibility.
* \param[out] pga_level       The PGA level currently used within the range [0 - 7].
*
* \return If the specified endpoint is compatible to this implementation the
*         function returns 0. If the endpoint is not compatible, a negative
*         error code is returned.
*/
int32_t ep_radar_p2g_get_pga_level(int32_t protocol_handle,
										  uint8_t endpoint);
			
/** @} */
										  
/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

/* End of include guard */
#endif /* ENDPOINTRADARP2G_H_INCLUDED */

/* --- End of File -------------------------------------------------------- */
