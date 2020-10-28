/**
 * \file EndpointTargetDetection.c
 *
 * \brief This file implements the API to communicate with Target Detection
 *        Endpoint in Infineon 24GHz radar devices.
 *
 * See header \ref EndpointTargetDetection.h for more information.
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
#include "EndpointTargetDetection.h"
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
 * \defgroup EndpointTargetDetectionCommandCodes
 *
 * \brief These are the command codes to identify the payload type.
 *
 * Each payload message of the target detection endpoint starts with one of these
 * command codes.
 *
 * @{
 */
 
#define  MSG_GET_DSP_SETTINGS           0x00  /**< A message to retrieve DSP settings */

#define  MSG_SET_DSP_SETTINGS           0x01  /**< A message to set DSP settings */

#define  MSG_GET_TARGETS                0x02  /**< A message to get info about targets detected */

#define  MSG_GET_RANGE_THRESHOLD		0x03  /**< A message to get target detection range threshold */

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

static int32_t parse_dsp_settings(int32_t protocol_handle, uint8_t endpoint,
                                  const uint8_t* payload,
                                  uint16_t payload_size);

static int32_t parse_target_info(int32_t protocol_handle, uint8_t endpoint,
                                 const uint8_t* payload,
                                 uint16_t payload_size);
								 
static int32_t parse_range_threshold(int32_t protocol_handle, uint8_t endpoint,
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
 *        module Protocol.c
 *
 * The structure contains the endpoint type and version range that is
 * supported by this module. The number identifying the endpoint type is the
 * ASCII code for 'RTDC' (= Radar Target DeteCtion).
 *
 * This struct contains also pointers to the functions to parse payload
 * messages and to retrieve human readable status/error messages from status
 * and error codes. Those function are called from the main module \ref Protocol.c
 */
const Endpoint_Definition_t ep_target_detection_definition =
{
    /*.type             =*/ 0x52544443, /* ASCII code 'RTDC'*/
    /*.min_version      =*/ 1,
    /*.max_version      =*/ 1,
    /*.description      =*/ "ifxRadar Target Detection",
    /*.parse_payload    =*/ parse_payload,
    /*.get_status_descr =*/ ep_radar_get_error_code_description
};

/**
 * \internal
 * \defgroup EndpointTargetDetectionRegisteredCallbacks
 *
 * \brief The registered callback function are stored here.
 *
 * For each message type that is expected from the connected device a callback
 * function cas be registered. That registered callback function is stored
 * here along with a data pointer that is forwarded the callback function each
 * time it is issued.
 *
 * @{
 */

/**
 * \brief The callback function to handle dsp settings message.
 */
static Callback_Dsp_Settings_t callback_dsp_settings = NULL;
/**
 * \brief The context data pointer for the dsp settings message callback
 *        function.
 */
static void* context_dsp_settings = NULL;

/**
 * \brief The callback function to handle targets message.
 */
static Callback_Target_Processing_t callback_target_processing = NULL;
/**
 * \brief The context data pointer for the radar target data message callback
 *        function.
 */
static void* context_target_processing = NULL;

/**
 * \brief The callback function to handle range threshold message.
 */
static Callback_Range_Threshold_t callback_range_threshold = NULL;
/**
 * \brief The context data pointer for the radar range threshold message callback
 *        function.
 */
static void* context_range_threshold = NULL;

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
    if (parse_dsp_settings(protocol_handle, endpoint, payload, payload_size))
        return;

    if (parse_target_info(protocol_handle, endpoint, payload, payload_size))
        return;
	
	if (parse_range_threshold(protocol_handle, endpoint, payload, payload_size))
        return;
}

static int32_t parse_target_info(int32_t protocol_handle, uint8_t endpoint,
                                 const uint8_t* payload,
                                 uint16_t payload_size)
{
    if ((protocol_read_payload_uint8(payload, 0) == MSG_GET_TARGETS) &&
        (payload_size >= 2))
    {
        // callbacks for target info are not defined yet
        if (callback_target_processing)
        {
            Target_Info_t* targets;
            uint8_t i;

            uint8_t num_targets = protocol_read_payload_uint8(payload, 1);

            const uint16_t target_data_size = sizeof(float) * 7 +
                                              sizeof(uint32_t);

            if (payload_size  == num_targets * target_data_size + 2)
            {
                targets = (Target_Info_t*)malloc(num_targets *
                                                 sizeof(Target_Info_t));

                for (i = 0; i < num_targets; ++i)
                {
                    targets[i] = *(Target_Info_t*)(payload + 2 +
                                                   i * target_data_size);
                }

                /* send target info to callback */
                callback_target_processing(context_target_processing,
                                           protocol_handle, endpoint,
                                           targets, num_targets);

                free (targets);
            }
        }
        return 1;
    }
    return 0;
}

static int32_t parse_dsp_settings(int32_t protocol_handle, uint8_t endpoint,
                                  const uint8_t* payload, uint16_t payload_size)
{
    if ((protocol_read_payload_uint8(payload, 0) == MSG_GET_DSP_SETTINGS))       
    {
        if (callback_dsp_settings)
        {
            DSP_Settings_t dsp_settings;
				
            dsp_settings.range_mvg_avg_length = *(uint8_t*) (payload + 1);			
						
            dsp_settings.min_range_cm      	  = *(uint16_t*)(payload + 2);
            dsp_settings.max_range_cm      	  = *(uint16_t*)(payload + 4);
						
            dsp_settings.min_speed_kmh     	  = *(uint16_t*)(payload + 6);
            dsp_settings.max_speed_kmh 	      = *(uint16_t*)(payload + 8);

			dsp_settings.min_angle_degree     = *(uint16_t*)(payload + 10);
            dsp_settings.max_angle_degree     = *(uint16_t*)(payload + 12);
			
			dsp_settings.range_threshold      = *(uint16_t*)(payload + 14);
            dsp_settings.speed_threshold      = *(uint16_t*)(payload + 16);
				        												
			if(payload_size == 27 )
			{
				// skip uint16 entry for index 18, see MMWSW-603
			    dsp_settings.enable_tracking      = *(uint8_t*) (payload + 20);
                dsp_settings.num_of_tracks        = *(uint8_t*) (payload + 21);
			
				dsp_settings.median_filter_length = *(uint8_t*) (payload + 22);
				dsp_settings.enable_mti_filter    = *(uint8_t*) (payload + 23);
			
			    dsp_settings.mti_filter_length    = *(uint16_t*)(payload + 24);
				// skip uint8 entry for index 26, see MMWSW-603												
			}												
			else	// Filled with default values, instead of reading the garbage memory
			{
				dsp_settings.enable_tracking      = 0;
                dsp_settings.num_of_tracks        = 1;
			
				dsp_settings.median_filter_length = 5;
				dsp_settings.enable_mti_filter    = 0;
			
			    dsp_settings.mti_filter_length    = 10;
			}
			         
			
            /* send DSP settings to callback */
            callback_dsp_settings(context_dsp_settings,
                                  protocol_handle, endpoint,
                                  &dsp_settings);
        }
        return 1;
    }
    return 0;
}

static int32_t parse_range_threshold(int32_t protocol_handle, uint8_t endpoint,
                                     const uint8_t* payload, uint16_t payload_size)
{
    if ((protocol_read_payload_uint8(payload, 0) == MSG_GET_RANGE_THRESHOLD) &&
        (payload_size == 3))
    {
        if (callback_range_threshold)
        {
            uint16_t threshold;
				
            threshold = *(uint16_t*) (payload + 1);			
			
            /* send DSP settings to callback */
            callback_range_threshold(context_range_threshold,
                                     protocol_handle, endpoint,
                                     threshold);
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

int32_t ep_targetdetect_is_compatible_endpoint(int32_t protocol_handle,
                                               uint8_t endpoint)
{
    return protocol_is_endpoint_compatible(protocol_handle, endpoint,
                                           &ep_target_detection_definition);
}

void ep_targetdetect_set_callback_dsp_settings(Callback_Dsp_Settings_t
                                                 callback,
                                               void* context)
{
    callback_dsp_settings = callback;
    context_dsp_settings = context;
}

void ep_targetdetect_set_callback_target_processing(Callback_Target_Processing_t
                                                        callback,
                                                    void* context)
{
    callback_target_processing = callback;
    context_target_processing = context;
}

void ep_targetdetect_set_callback_range_threshold(Callback_Range_Threshold_t
                                                  callback,
                                                  void* context)
{
    callback_range_threshold = callback;
    context_range_threshold = context;
}

int32_t ep_targetdetect_set_dsp_settings(int32_t protocol_handle,
                                         uint8_t endpoint,
                                         const DSP_Settings_t* dsp_settings)
{
    /* build message to send */
    uint8_t cmd_message[27];

    protocol_write_payload_uint8 (cmd_message,  0, MSG_SET_DSP_SETTINGS); 
	
	protocol_write_payload_uint8 (cmd_message,  1, dsp_settings->range_mvg_avg_length);
		
    protocol_write_payload_uint16(cmd_message,  2, dsp_settings->min_range_cm);
    protocol_write_payload_uint16(cmd_message,  4, dsp_settings->max_range_cm);
    protocol_write_payload_uint16(cmd_message,  6, dsp_settings->min_speed_kmh);
    protocol_write_payload_uint16(cmd_message,  8, dsp_settings->max_speed_kmh);
	protocol_write_payload_uint16(cmd_message, 10, dsp_settings->min_angle_degree);
    protocol_write_payload_uint16(cmd_message, 12, dsp_settings->max_angle_degree);
	protocol_write_payload_uint16(cmd_message, 14, dsp_settings->range_threshold);
    protocol_write_payload_uint16(cmd_message, 16, dsp_settings->speed_threshold);
	
	protocol_write_payload_uint16(cmd_message, 18, 0 );
	protocol_write_payload_uint8 (cmd_message, 20, dsp_settings->enable_tracking);
	protocol_write_payload_uint8 (cmd_message, 21, dsp_settings->num_of_tracks);
	
	protocol_write_payload_uint8 (cmd_message, 22, dsp_settings->median_filter_length);
	protocol_write_payload_uint8 (cmd_message, 23, dsp_settings->enable_mti_filter);	
	
	protocol_write_payload_uint16(cmd_message, 24, dsp_settings->mti_filter_length);
	protocol_write_payload_uint16(cmd_message, 26, 0 );
	

    /* send message and process received response */
    return protocol_send_and_receive(protocol_handle, endpoint,
                                     &ep_target_detection_definition,
                                     cmd_message, sizeof(cmd_message));
}

int32_t ep_targetdetect_get_dsp_settings(int32_t protocol_handle,
                                          uint8_t endpoint)
{
    /* build message to send */
    uint8_t cmd_message[1];

    protocol_write_payload_uint8 (cmd_message, 0, MSG_GET_DSP_SETTINGS);

    /* send message and process received response */
    return protocol_send_and_receive(protocol_handle, endpoint,
                                     &ep_target_detection_definition,
                                     cmd_message, sizeof(cmd_message));
}

int32_t ep_targetdetect_get_targets(int32_t protocol_handle, uint8_t endpoint)
{
    /* build message to send */
    uint8_t cmd_message[1];

    protocol_write_payload_uint8 (cmd_message, 0, MSG_GET_TARGETS);

    /* send message and process received response */
    return protocol_send_and_receive(protocol_handle, endpoint,
                                     &ep_target_detection_definition,
                                     cmd_message, sizeof(cmd_message));
}

int32_t ep_targetdetect_get_range_threshold(int32_t protocol_handle, uint8_t endpoint)
{
    /* build message to send */
    uint8_t cmd_message[1];

    protocol_write_payload_uint8 (cmd_message, 0, MSG_GET_RANGE_THRESHOLD);

    /* send message and process received response */
    return protocol_send_and_receive(protocol_handle, endpoint,
                                     &ep_target_detection_definition,
                                     cmd_message, sizeof(cmd_message));
}

/* --- End of File -------------------------------------------------------- */
