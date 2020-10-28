/**
 * \file EndpointRadarADCXMC.c
 *
 * \brief This file implements the API to communicate with Radar ADC XMC
 *        Endpoints in Infineon sensor devices.
 *
 * See header \ref EndpointRadarADCXMC.h for more information.
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
#include "EndpointRadarADCXMC.h"
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
 * \defgroup EndpointRadarADCXMCCommandCodes
 *
 * \brief These are the command codes to identify the payload type.
 *
 * Each payload message of the supported endpoint starts with one of these
 * commend codes.
 *
 * @{
 */
#define MSG_GET_CONFIGURATION     0x00 /**< A message to query the ADC
                                            configuration. */
#define MSG_SET_CONFIGURATION     0x01 /**< A message containing the ADC
                                            configuration. */
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

static int32_t parse_adc_configuration(int32_t protocol_handle,
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
 * ASCII code for 'RADX' (= ifxRadar ADC XMC).
 *
 * This struct contains also pointers to the functions to parse payload
 * messages and retrieve human readable status messages. Those function are
 * called from the main module \ref Protocol.c
 */
const Endpoint_Definition_t ep_radar_adcxmc_definition =
{
    /*.type             =*/ 0x52414458,
    /*.min_version      =*/ 1,
    /*.max_version      =*/ 1,
    /*.description      =*/ "ifxRadar ADC (XMC)",
    /*.parse_payload    =*/ parse_payload,
    /*.get_status_descr =*/ ep_radar_get_error_code_description
};

/**
 * \internal
 * \defgroup EndpointRadarADCXMCRegisteredCallbacks
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
 * \brief The callback function to handle ADC configuration messages.
 */
static Callback_Adc_Xmc_Configuration_t callback_adc_configuration = NULL;

/**
 * \brief The context data pointer for the ADC configuration message callback
 *        function.
 */
static void* context_adc_configuration = NULL;

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
     * was successful
     */
    if (parse_adc_configuration(protocol_handle, endpoint,
                                payload, payload_size))
    {
        return;
    }
}

static int32_t parse_adc_configuration(int32_t protocol_handle,
                                       uint8_t endpoint,
                                       const uint8_t* payload,
                                       uint16_t payload_size)
{
    if ((protocol_read_payload_uint8(payload, 0) == MSG_SET_CONFIGURATION) &&
        (payload_size == 7))
    {
        if (callback_adc_configuration)
        {
            Adc_Xmc_Configuration_t adc_configuration;

            adc_configuration.samplerate_Hz =
                                     protocol_read_payload_uint32(payload, 1);
            adc_configuration.resolution  =
                                     protocol_read_payload_uint8 (payload, 5);
            adc_configuration.use_post_calibration =
                                     protocol_read_payload_uint8 (payload, 6);

            /* send ADC Configuration to callback */
            callback_adc_configuration(context_adc_configuration,
                                       protocol_handle, endpoint,
                                       &adc_configuration);
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

int32_t ep_radar_adcxmc_is_compatible_endpoint(int32_t protocol_handle,
                                               uint8_t endpoint)
{
    return protocol_is_endpoint_compatible(protocol_handle, endpoint,
                                           &ep_radar_adcxmc_definition);
}

void ep_radar_adcxmc_set_callback_adc_configuration(Callback_Adc_Xmc_Configuration_t
                                                      callback,
                                                    void* context)
{
    callback_adc_configuration = callback;
    context_adc_configuration = context;
}

int32_t ep_radar_adcxmc_set_adc_configuration(int32_t protocol_handle,
                                              uint8_t endpoint,
                                              const Adc_Xmc_Configuration_t*
                                                adc_configuration)
{
    /* build message to send */
    uint8_t cmd_message[7];

    protocol_write_payload_uint8 (cmd_message, 0, MSG_SET_CONFIGURATION);
    protocol_write_payload_uint32(cmd_message, 1, adc_configuration->samplerate_Hz);
    protocol_write_payload_uint8 (cmd_message, 5, adc_configuration->resolution);
    protocol_write_payload_uint8 (cmd_message, 6, adc_configuration->use_post_calibration);

    /* send message and process received response */
    return protocol_send_and_receive(protocol_handle, endpoint,
                                     &ep_radar_adcxmc_definition,
                                     cmd_message, sizeof(cmd_message));
}

int32_t ep_radar_adcxmc_get_adc_configuration(int32_t protocol_handle,
                                              uint8_t endpoint)
{
    /* build message to send */
    uint8_t cmd_message[1];

    protocol_write_payload_uint8(cmd_message,0, MSG_GET_CONFIGURATION);

    /* send message and process received response */
    return protocol_send_and_receive(protocol_handle, endpoint,
                                     &ep_radar_adcxmc_definition,
                                     cmd_message, sizeof(cmd_message));
}

/* --- End of File -------------------------------------------------------- */
