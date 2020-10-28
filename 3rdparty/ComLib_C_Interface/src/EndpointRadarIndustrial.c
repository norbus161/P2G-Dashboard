/**
 * \file EndpointRadarIndustrial.c
 *
 * \brief This file implements the API to communicate with radar industrial
 *        Endpoint in Infineon 24GHz radar devices.
 *
 * See header \ref EndpointRadarIndustrial.h for more information.
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

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/
#include "EndpointRadarIndustrial.h"
#define __PROTOCOL_INCLUDE_ENDPOINT_ONLY_API__
#include "Protocol_internal.h"
#undef __PROTOCOL_INCLUDE_ENDPOINT_ONLY_API__
#include <stdlib.h>
#include <string.h>

/*
==============================================================================
   2. LOCAL DEFINITIONS
==============================================================================
*/

/**
 * \internal
 * \defgroup EndpointRadarIndustrialCommandCodes
 *
 * \brief The radar industrial endpoint defined the following commands. 
 *
 * Each payload message of the radar industrial endpoint starts with one of these
 * command codes.
 *
 * @{
 */

#define MSG_DISABLE_DUTY_CYCLE 			0x00  	/**< A message to disable duty cycle. */

#define MSG_ENABLE_DUTY_CYCLE			0x01  	/**< A message to enable duty cycle. */	

#define MSG_IS_ENABLE_DUTY_CYCLE		0x02  	/**< A message to check the status of duty cycle. */

#define MSG_DISABLE_BGT_LNA 			0x03  	/**< A message to disable BGT Rx LNA gain. */

#define MSG_ENABLE_BGT_LNA				0x04  	/**< A message to enable BGT Rx LNA gain. */

#define MSG_IS_ENABLE_BGT_LNA			0x05  	/**< A message to check the status of BGT Rx LNA gain. */

/** @} */

/*
==============================================================================
   5. LOCAL FUNCTION PROTOTYPES
==============================================================================
*/
/**
 * \internal
 * See module \ref EndpointRadarErrorCodes.c for information.
 */
extern const char* ep_radar_get_error_code_description(uint16_t error_code);

/**
 * \internal
 * \brief This function is called to parse a payload message.
 *
 * Whenever a connected device sends a payload message from an endpoint that
 * is of type and version compatible to the implementation is this module,
 * this function is invoked to parse the content of that message.
 *
 * \param[in] protocol_handle  The handle to the connection, the message was
 *                             received through.
 * \param[in] endpoint         The endpoint the message was sent from.
 * \param[in] payload          A pointer to the buffer containing the payload
 *                             to parse.
 * \param[in] payload_size     The number of bytes in payload.
 */
static void parse_payload(int32_t protocol_handle, uint8_t endpoint,
                          const uint8_t* payload, uint16_t payload_size);

/*
==============================================================================
   4. DATA
==============================================================================
*/
/**
 * \internal
 * \brief This structure contains information about this module needed by the
 *        module Protocol.c
 *
 * The structure contains the endpoint type and version range that is
 * supported by this module. The number identifying the endpoint type is the
 * ASCII code for 'RIND' (= Radar Industrial).
 *
 * This struct contains also pointers to the functions to parse payload
 * messages and to retrieve human readable status/error messages from status
 * and error codes. Those function are called from the main module \ref Protocol.c
 */
const Endpoint_Definition_t ep_radar_industrial_definition =
{
    /*.type             =*/ 0x52494E44, /* ASCII code 'RIND' = Radar Industrial */
    /*.min_version      =*/ 1,
    /*.max_version      =*/ 1,
    /*.description      =*/ "ifxRadar Industrial",
    /*.parse_payload    =*/ parse_payload,
    /*.get_status_descr =*/ ep_radar_get_error_code_description
};

/**
 * \internal
 * \defgroup EndpointRadarP2GRegisteredCallbacks
 *
 * \brief The registered callback function are stored here.
 *
 * For each message type that is expected from the connected device a callback
 * function can be registered. That registered callback function is stored
 * here along with a data pointer that is forwarded the callback function each
 * time it is issued.
 *
 * @{
 */

/**
* \brief The callback function to handle Industrial radar Duty Cycle status messages.
*/
static Callback_duty_cycle_status_t callback_duty_cycle_status = NULL;

/**
* \brief The context data pointer for the Industrial radar Duty Cycle status message callback
*        function.
*/
static void* context_duty_cycle_status = NULL;

/**
* \brief The callback function to handle Industrial radar BGT LNA status messages.
*/
static Callback_bgt_lna_status_t callback_bgt_lna_status = NULL;

/**
* \brief The context data pointer for the Industrial radar BGT LNA status message callback
*        function.
*/
static void* context_bgt_lna_status = NULL;

/** @} */

/*
==============================================================================
  6. LOCAL FUNCTIONS
==============================================================================
*/

static int32_t parse_duty_cycle_status(int32_t protocol_handle, uint8_t endpoint,
								const uint8_t* payload, uint16_t payload_size)
{
	if ((protocol_read_payload_uint8(payload, 0) == MSG_IS_ENABLE_DUTY_CYCLE) &&
		(payload_size == 2))
	{
		if (callback_duty_cycle_status)
		{
			uint8_t  duty_cycle_is_enabled;

			duty_cycle_is_enabled = protocol_read_payload_uint8(payload, 1);
			
			duty_cycle_is_enabled = (duty_cycle_is_enabled > 0 ? 1 : 0);

			/* send frame format to callback */
			callback_duty_cycle_status(context_duty_cycle_status, protocol_handle,
									   endpoint, duty_cycle_is_enabled);
		}
		return 1;
	}
	return 0;
}

static int32_t parse_bgt_lna_status(int32_t protocol_handle, uint8_t endpoint,
							   const uint8_t* payload, uint16_t payload_size)
{
	if ((protocol_read_payload_uint8(payload, 0) == MSG_IS_ENABLE_BGT_LNA) &&
		(payload_size == 2))
	{
		if (callback_bgt_lna_status)
		{
			uint8_t  bgt_lna_is_enabled;

			bgt_lna_is_enabled = protocol_read_payload_uint8(payload, 1);
			
			bgt_lna_is_enabled = (bgt_lna_is_enabled > 0 ? 1 : 0);

			/* send frame format to callback */
			callback_bgt_lna_status(context_bgt_lna_status, protocol_handle,
									endpoint, bgt_lna_is_enabled);
		}
		return 1;
	}
	return 0;
}

static void parse_payload(int32_t protocol_handle, uint8_t endpoint,
                          const uint8_t* payload, uint16_t payload_size)
{
    /* try to parse payload for all supported message types, stop when parsing
     * was successful
     */
	if (parse_bgt_lna_status(protocol_handle, endpoint,
		payload, payload_size))
		return;
		
	if (parse_duty_cycle_status(protocol_handle, endpoint,
		payload, payload_size))
		return;			
}

/*
==============================================================================
   7. EXPORTED FUNCTIONS
==============================================================================
*/

int32_t ep_radar_industrial_is_compatible_endpoint(int32_t protocol_handle,
												   uint8_t endpoint)
{
	return protocol_is_endpoint_compatible(protocol_handle, endpoint,
		&ep_radar_industrial_definition);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

void ep_radar_industrial_set_callback_duty_cycle_status(Callback_duty_cycle_status_t callback,
														void* context)
{
	callback_duty_cycle_status = callback;
	context_duty_cycle_status = context;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

void ep_radar_industrial_set_callback_bgt_lna_status(Callback_bgt_lna_status_t callback,
													 void* context)
{
	callback_bgt_lna_status = callback;
	context_bgt_lna_status = context;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

int32_t ep_radar_industrial_duty_cycle_is_enable(int32_t protocol_handle,
												 uint8_t endpoint)
{
	/* build message to send */
	uint8_t cmd_message[1];

	protocol_write_payload_uint8(cmd_message, 0, MSG_IS_ENABLE_DUTY_CYCLE);

	/* send message and process received response */
	return protocol_send_and_receive(protocol_handle, endpoint,
									&ep_radar_industrial_definition,
									cmd_message, sizeof(cmd_message));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

int32_t ep_radar_industrial_duty_cycle_enable(int32_t protocol_handle,
											  uint8_t endpoint)
{
    /* build message to send */
    uint8_t cmd_message[1];

    protocol_write_payload_uint8(cmd_message, 0, MSG_ENABLE_DUTY_CYCLE);

    /* send message and process received response */
    return protocol_send_and_receive(protocol_handle, endpoint,
                                     &ep_radar_industrial_definition,
                                     cmd_message, sizeof(cmd_message));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

int32_t ep_radar_industrial_duty_cycle_disable(int32_t protocol_handle,
											   uint8_t endpoint)
{
    /* build message to send */
    uint8_t cmd_message[1];

    protocol_write_payload_uint8(cmd_message, 0, MSG_DISABLE_DUTY_CYCLE);

    /* send message and process received response */
    return protocol_send_and_receive(protocol_handle, endpoint,
                                     &ep_radar_industrial_definition,
                                     cmd_message, sizeof(cmd_message));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

int32_t ep_radar_industrial_bgt_lna_is_enable(int32_t protocol_handle,
											  uint8_t endpoint)
{
	/* build message to send */
	uint8_t cmd_message[1];

	protocol_write_payload_uint8(cmd_message, 0, MSG_IS_ENABLE_BGT_LNA);

	/* send message and process received response */
	return protocol_send_and_receive(protocol_handle, endpoint,
									&ep_radar_industrial_definition,
									cmd_message, sizeof(cmd_message));
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

int32_t ep_radar_industrial_bgt_lna_enable(int32_t protocol_handle,
										   uint8_t endpoint)
{
    /* build message to send */
    uint8_t cmd_message[1];

    protocol_write_payload_uint8(cmd_message, 0, MSG_ENABLE_BGT_LNA);

    /* send message and process received response */
    return protocol_send_and_receive(protocol_handle, endpoint,
                                     &ep_radar_industrial_definition,
                                     cmd_message, sizeof(cmd_message));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

int32_t ep_radar_industrial_bgt_lna_disable(int32_t protocol_handle,
											uint8_t endpoint)
{
    /* build message to send */
    uint8_t cmd_message[1];

    protocol_write_payload_uint8(cmd_message, 0, MSG_DISABLE_BGT_LNA);

    /* send message and process received response */
    return protocol_send_and_receive(protocol_handle, endpoint,
                                     &ep_radar_industrial_definition,
                                     cmd_message, sizeof(cmd_message));
}

/* --- End of File -------------------------------------------------------- */
