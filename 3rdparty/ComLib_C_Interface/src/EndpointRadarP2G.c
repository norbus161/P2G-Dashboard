/**
 * \file EndpointRadarP2G.c
 *
 * \brief This file implements the API to communicate with Radar P2G
 *        Endpoint in Infineon Position2Go boards.
 *
 * See header \ref EndpointRadarP2G.h for more information.
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

#include "EndpointRadarP2G.h"
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
 * \defgroup EndpointRadarP2GCommandCodes
 *
 * \brief The P2G endpoint defined the following commands. 
 *
 * Each payload message of the radar P2G endpoint starts with one of these
 * command codes.
 *
 * @{
 */

#define MSG_GET_PGA_LEVEL 			0x00  	/**< A message to query current PGA gain level. */
											   
#define MSG_SET_PGA_LEVEL			0x01  	/**< A message containing PGA gain level. */

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
 * \brief This function returns a human readable description of status codes 
 * specifically defined by the radar P2G endpoint. If the error code definition is
 * not available in the radar P2G endpoint, then the generic function 
 * \ref ep_radar_get_error_code_description is called.
 * See module \ref EndpointRadarErrorCodes.c for information.
 *
 * \param[in] error_code  The status code to be described.
 *
 * \return This function returns a pointer to a zero-terminated string
 *         containing the human readable description. The caller must not free
 *         or modify the memory pointed to.
 */
const char* ep_radar_p2g_get_error_code_description(uint16_t error_code);
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
 * ASCII code for 'RP2G' (= Radar Position2Go).
 *
 * This struct contains also pointers to the functions to parse payload
 * messages and to retrieve human readable status/error messages from status
 * and error codes. Those function are called from the main module \ref Protocol.c
 */
const Endpoint_Definition_t ep_radar_p2g_definition =
{
    /*.type             =*/ 0x52503247, /* ASCII code 'RP2G' = Industrial Radar Position2Go */
    /*.min_version      =*/ 1,
    /*.max_version      =*/ 1,
    /*.description      =*/ "ifxRadar Position2Go",
    /*.parse_payload    =*/ parse_payload,
    /*.get_status_descr =*/ ep_radar_p2g_get_error_code_description
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
 * \brief The callback function to handle PGA gain level messages.
 */
static Callback_PGA_Level_t callback_pga_level = NULL;

/**
 * \brief The context data pointer for the PGA power level message callback
 *        function.
 */
static void* context_pga_level = NULL;

/** @} */

/*
==============================================================================
  6. LOCAL FUNCTIONS
==============================================================================
*/

static int32_t parse_pga_level(int32_t protocol_handle, uint8_t endpoint,
                               const uint8_t* payload,
                               uint16_t payload_size)
{
    if ((protocol_read_payload_uint8(payload, 0) == MSG_SET_PGA_LEVEL) &&
        (payload_size == 3))
    {
        if (callback_pga_level)
        {
            uint16_t  pga_level_val;

            pga_level_val = protocol_read_payload_uint16(payload,  1);
            
            /* send frame format to callback */
            callback_pga_level(context_pga_level, protocol_handle,
                                 endpoint, pga_level_val);
        }
        return 1;
    }
    return 0;
}

//===========================================================================
								 
static void parse_payload(int32_t protocol_handle, uint8_t endpoint,
                          const uint8_t* payload, uint16_t payload_size)
{
    /* try to parse payload for all supported message types, stop when parsing
     * was successful
     */
	if (parse_pga_level(protocol_handle, endpoint, payload, payload_size))
	{
		return;
	}
}			

const char* ep_radar_p2g_get_error_code_description(uint16_t error_code)
{
	switch(error_code)
	{
		case EP_RADAR_P2G_ERR_UNSUPPORTED_PGA_GAIN:
			return "The specified PGA gain level is not supported";
		default:
			return ep_radar_get_error_code_description(error_code);
	}
}


/*
==============================================================================
   7. EXPORTED FUNCTIONS
==============================================================================
*/

int32_t ep_radar_p2g_is_compatible_endpoint(int32_t protocol_handle,
												   uint8_t endpoint)
{
	return protocol_is_endpoint_compatible(protocol_handle, endpoint,
		&ep_radar_p2g_definition);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

void ep_radar_p2g_set_callback_pga_level(Callback_PGA_Level_t callback, void* context)
{
    callback_pga_level = callback;
    context_pga_level = context;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

int32_t ep_radar_p2g_set_pga_level(int32_t protocol_handle,
                                          uint8_t endpoint,
										  uint16_t pga_level)
{
    /* build message to send */
    uint8_t cmd_message[3];

    protocol_write_payload_uint8 (cmd_message, 0, MSG_SET_PGA_LEVEL);
	protocol_write_payload_uint16(cmd_message, 1, pga_level);

    /* send message and process received response */
    return protocol_send_and_receive(protocol_handle, endpoint,
                                     &ep_radar_p2g_definition,
                                     cmd_message, sizeof(cmd_message));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

int32_t ep_radar_p2g_get_pga_level(int32_t protocol_handle,
                                          uint8_t endpoint)
{
    /* build message to send */
    uint8_t cmd_message[1];

    protocol_write_payload_uint8 (cmd_message, 0, MSG_GET_PGA_LEVEL);
	
    /* send message and process received response */
    return protocol_send_and_receive(protocol_handle, endpoint,
                                     &ep_radar_p2g_definition,
                                     cmd_message, sizeof(cmd_message));
}

/* --- End of File -------------------------------------------------------- */
