/**
 * \file EndpointRadarFMCW.c
 *
 * \brief This file implements the API to communicate with Radar Endpoints in
 *        Infineon sensor devices.
 *
 * See header \ref EndpointRadarFMCW.h for more information.
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

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/
#include "EndpointRadarFMCW.h"
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
 * \defgroup EndpointRadarFMCWCommandCodes
 *
 * \brief These are the command codes to identify the payload type.
 *
 * Each payload message of the supported endpoint starts with one of these
 * commend codes.
 *
 * @{
 */
#define MSG_GET_CONFIGURATION     0x00 /**< A message to query the FMCW
                                            configuration. */
#define MSG_SET_CONFIGURATION     0x01 /**< A message containing the FMCW
                                            configuration. */
#define MSG_GET_BW_PER_SECOND     0x02 /**< A message to query the bandwidth
                                            per second. */
#define MSG_SET_BW_PER_SECOND     0x02 /**< A message containing the bandwidth
                                            per second. */
/** @} */

/*
==============================================================================
   5. LOCAL FUNCTION PROTOTYPES
==============================================================================
*/

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

/**
 * \internal
 * See module \ref EndpointRadarErrorCodes.c for information.
 */
extern const char* ep_radar_get_error_code_description(uint16_t error_code);

static int32_t parse_fmcw_configuration(int32_t protocol_handle,
                                        uint8_t endpoint,
                                        const uint8_t* payload,
                                        uint16_t payload_size);

static int32_t parse_bandwidth_per_second(int32_t protocol_handle,
                                          uint8_t endpoint,
                                          const uint8_t* payload,
                                          uint16_t payload_size);

/*
==============================================================================
   4. DATA
==============================================================================
*/

/**
 * \internal
 * \brief This structure contains information about this module needed by the
 * module Protocol.c
 *
 * The structure contains the endpoint type and version range that is
 * supported by this module. The number identifying the endpoint type is the
 * ASCII code for 'RFMC' (= ifxRadar FMCW).
 *
 * This struct contains also pointers to the functions to parse payload
 * messages and retrieve human readable status messages. Those function are
 * called from the main module \ref Protocol.c
 */
const Endpoint_Definition_t ep_radar_fmcw_definition =
{
    /*.type             =*/ 0x52464D43,
    /*.min_version      =*/ 1,
    /*.max_version      =*/ 1,
    /*.description      =*/ "ifxRadar FMCW",
    /*.parse_payload    =*/ parse_payload,
    /*.get_status_descr =*/ ep_radar_get_error_code_description
};

/**
 * \internal
 * \defgroup EndpointRadarFMCWRegisteredCallbacks
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
 * \brief The callback function to handle FMCW RF configuration messages.
 */
static Callback_Fmcw_Configuration_t callback_fmcw_configuration = NULL;

/**
 * \brief The context data pointer for the FMCW RF configuration message
 *        callback function.
 */
static void* context_fmcw_configuration = NULL;

/**
 * \brief The callback function to handle bandwidth per second messages.
 */
static Callback_Bandwidth_Per_Second_t callback_bandwidth_per_second = NULL;

/**
 * \brief The context data pointer for the bandwidth per second message
 *        callback function.
 */
static void* context_bandwidth_per_second = NULL;

/** @} */

/*
==============================================================================
  6. LOCAL FUNCTIONS
==============================================================================
*/

static void parse_payload(int32_t protocol_handle, uint8_t endpoint,
                          const uint8_t* payload, uint16_t payload_size)
{
    /* try to parse payload for all supported message types, stop when parsing
     * was successful */
    if (parse_bandwidth_per_second(protocol_handle, endpoint,
                                   payload, payload_size))
    {
        return;
    }

    if (parse_fmcw_configuration(protocol_handle, endpoint,
                                 payload, payload_size))
    {
        return;
    }
}

static int32_t parse_fmcw_configuration(int32_t protocol_handle,
                                        uint8_t endpoint,
                                        const uint8_t* payload,
                                        uint16_t payload_size)
{
    if ((protocol_read_payload_uint8(payload, 0) == MSG_SET_CONFIGURATION) &&
        (payload_size == 11))
    {
        if (callback_fmcw_configuration)
        {
            Fmcw_Configuration_t sFMCWConfiguration;

            sFMCWConfiguration.lower_frequency_kHz =
                                    protocol_read_payload_uint32(payload,  1);
            sFMCWConfiguration.upper_frequency_kHz =
                                    protocol_read_payload_uint32(payload,  5);
            sFMCWConfiguration.direction =
                 (Chirp_Direction_t)protocol_read_payload_uint8 (payload,  9);
            sFMCWConfiguration.tx_power =
                                    protocol_read_payload_uint8 (payload, 10);

            /* send RF Configuration to callback */
            callback_fmcw_configuration(context_fmcw_configuration,
                                        protocol_handle, endpoint,
                                        &sFMCWConfiguration);
        }

        return 1;
    }

    return 0;
}

static int32_t parse_bandwidth_per_second(int32_t protocol_handle,
                                          uint8_t endpoint,
                                          const uint8_t* payload,
                                          uint16_t payload_size)
{
    if ((protocol_read_payload_uint8(payload, 0) == MSG_SET_BW_PER_SECOND) &&
        (payload_size == 5))
    {
        if (callback_bandwidth_per_second)
        {
            uint32_t uBandwidthPerSecond;

            uBandwidthPerSecond = protocol_read_payload_uint32(payload, 1);

            /* send chirp timing to callback */
            callback_bandwidth_per_second(context_bandwidth_per_second,
                                          protocol_handle, endpoint,
                                          uBandwidthPerSecond);
        }

        return 1;
    }

    return 0;
}
/*
==============================================================================
   7. EXPORTED FUNCTIONS
==============================================================================
*/

int32_t ep_radar_fmcw_is_compatible_endpoint(int32_t protocol_handle,
                                             uint8_t endpoint)
{
    return protocol_is_endpoint_compatible(protocol_handle, endpoint,
                                           &ep_radar_fmcw_definition);
}

void ep_radar_fmcw_set_callback_fmcw_configuration(Callback_Fmcw_Configuration_t
                                                     callback,
                                                   void* context)
{
    callback_fmcw_configuration = callback;
    context_fmcw_configuration = context;
}

void ep_radar_fmcw_set_callback_bandwidth_per_second(Callback_Bandwidth_Per_Second_t
                                                       callback,
                                                     void* context)
{
    callback_bandwidth_per_second = callback;
    context_bandwidth_per_second = context;
}

int32_t ep_radar_fmcw_set_fmcw_configuration(int32_t protocol_handle,
                                             uint8_t endpoint,
                                             const Fmcw_Configuration_t*
                                               fmcw_configuration)
{
    /* build message to send */
    uint8_t cmd_message[11];

    protocol_write_payload_uint8 (cmd_message,  0, MSG_SET_CONFIGURATION);
    protocol_write_payload_uint32(cmd_message,  1, fmcw_configuration->lower_frequency_kHz);
    protocol_write_payload_uint32(cmd_message,  5, fmcw_configuration->upper_frequency_kHz);
    protocol_write_payload_uint8 (cmd_message,  9, fmcw_configuration->direction);
    protocol_write_payload_uint8 (cmd_message, 10, fmcw_configuration->tx_power);

    /* send message and process received response */
    return protocol_send_and_receive(protocol_handle, endpoint,
                                     &ep_radar_fmcw_definition,
                                     cmd_message, sizeof(cmd_message));
}

int32_t ep_radar_fmcw_get_fmcw_configuration(int32_t protocol_handle,
                                             uint8_t endpoint)
{
    /* build message to send */
    uint8_t cmd_message[1];

    protocol_write_payload_uint8(cmd_message, 0, MSG_GET_CONFIGURATION);

    /* send message and process received response */
    return protocol_send_and_receive(protocol_handle, endpoint,
                                     &ep_radar_fmcw_definition,
                                     cmd_message, sizeof(cmd_message));
}

int32_t ep_radar_fmcw_get_bandwidth_per_second(int32_t protocol_handle,
                                               uint8_t endpoint)
{
    /* build message to send */
    uint8_t cmd_message[1];

    protocol_write_payload_uint8(cmd_message, 0, MSG_GET_BW_PER_SECOND);

    /* send message and process received response */
    return protocol_send_and_receive(protocol_handle, endpoint,
                                     &ep_radar_fmcw_definition,
                                     cmd_message, sizeof(cmd_message));
}

/* --- End of File -------------------------------------------------------- */
