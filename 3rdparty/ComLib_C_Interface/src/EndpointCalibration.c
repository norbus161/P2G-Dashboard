/**
 * \file EndpointCalibration.c
 *
 * \brief This file implements the API to communicate with Calibration
 *        Endpoint in Infineon 24GHz radar devices.
 *
 * See header \ref EndpointCalibration.h for more information.
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
#include "EndpointCalibration.h"
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
 * \defgroup EndpointCalibrationCommandCodes
 *
 * \brief These are the command codes to identify the payload type.
 *
 * Each payload message of the calibration endpoint starts with one of these
 * command codes.
 *
 * @{
 */

#define  MSG_SET_ADC_FLASH_CALIBRATION		0x01	/**< A message to save ADC calibration data from EEPROM */

#define  MSG_GET_ADC_FLASH_CALIBRATION     	0x02	/**< A message to read ADC calibration data from EEPROM */

#define  MSG_CLEAR_ADC_FLASH_CALIBRATION    0x03	/**< A message to delete ADC calibration data from EEPROM */

#define  MSG_SET_ADC_SRAM_CALIBRATION      	0x04	/**< A message to save ADC calibration data in SRAM */

#define  MSG_GET_ADC_SRAM_CALIBRATION      	0x05	/**< A message to read ADC calibration data in SRAM */

#define  MSG_CLEAR_ADC_SRAM_CALIBRATION     0x06	/**< A message to delete ADC calibration data from SRAM */

#define  MSG_SET_ALGO_FLASH_CALIBRATION		0x07  	/**< A message to store the Algo calibration in the Flash memory. */

#define  MSG_GET_ALGO_FLASH_CALIBRATION		0x08  	/**< A message to read current Algo calibration data stored in the Flash memory. */

#define  MSG_CLEAR_ALGO_FLASH_CALIBRATION	0x09  	/**< A message to clear the Algo calibration data from Flash memory. */

#define  MSG_SET_ALGO_SRAM_CALIBRATION		0x0A  	/**< A message to store the Algo calibration data in the SRAM memory. */

#define  MSG_GET_ALGO_SRAM_CALIBRATION		0x0B  	/**< A message to read current Algo calibration data stored in the SRAM memory. */

#define  MSG_CLEAR_ALGO_SRAM_CALIBRATION	0x0C  	/**< A message to clear the Algo calibration data from SRAM memory. */

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
						
static int32_t parse_adc_calibration_data(int32_t protocol_handle,
						uint8_t endpoint, const uint8_t* payload,
						uint16_t payload_size);
										  
static int32_t parse_algo_calibration_data(int32_t protocol_handle,
						uint8_t endpoint, const uint8_t* payload,
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
 * ASCII code for 'RCAL' (= Radar Calibration).
 *
 * This struct contains also pointers to the functions to parse payload
 * messages and retrieve human readable status messages. Those function are
 * called from the main module \ref Protocol.c
 */
const Endpoint_Definition_t ep_calibration_definition =
{
    /*.type             =*/ 0x5243414C, /* ASCII code 'RCAL'*/
    /*.min_version      =*/ 1,
    /*.max_version      =*/ 1,
    /*.description      =*/ "ifxRadar Calibration",
    /*.parse_payload    =*/ parse_payload,
    /*.get_status_descr =*/ ep_radar_get_error_code_description
};

/**
 * \internal
 * \defgroup EndpointCalibrationRegisteredCallbacks
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
 * \brief The callback function to handle ADC Calibration data messages.
 */
static Callback_ADC_Calibration_Data_t callback_adc_calibration_data = NULL;

/**
 * \brief The context data pointer for the ADC Calibration data messages
 *        callback function.
 */
static void* context_adc_calibration_data = NULL;

/**
 * \brief The callback function to handle Algo Calibration data messages.
 */
static Callback_Algo_Calibration_Data_t callback_algo_calibration_data = NULL;

/**
 * \brief The context data pointer for the Algo Calibration data messages
 *        callback function.
 */
static void* context_algo_calibration_data = NULL;

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
    if (parse_adc_calibration_data(protocol_handle, endpoint,
								payload, payload_size))
	{
        return;
	}
	
	if (parse_algo_calibration_data(protocol_handle, endpoint,
								payload, payload_size))
	{
		return;
	}
}

//--------------------------------------------------------------------------------

static int32_t parse_adc_calibration_data(int32_t protocol_handle,
						uint8_t endpoint, const uint8_t* payload,
						uint16_t payload_size)
{
    if (((protocol_read_payload_uint8(payload, 0) == MSG_GET_ADC_FLASH_CALIBRATION) || 
		(protocol_read_payload_uint8( payload, 0) == MSG_GET_ADC_SRAM_CALIBRATION)) && 
		(payload_size >= 2))
    {
        if (callback_adc_calibration_data)
        {
            uint16_t num_of_bytes;
            uint16_t total_samples;
            uint16_t current_sample;
            uint16_t read_idx = 3;
            uint16_t sample_bit_mask;
            float norm_factor;
            float* sample_data;
            float* write_ptr;

            num_of_bytes = protocol_read_payload_uint16(payload,  1);
            total_samples = (num_of_bytes >> 1);

            sample_bit_mask = (1 << 12) - 1;
            norm_factor = 1.0f / (float)(sample_bit_mask);

            if (payload_size  == read_idx + total_samples * 2)
            {
                sample_data = (float*)malloc(total_samples * sizeof(float));
                write_ptr = sample_data;

                while (total_samples)
                {
                    current_sample = protocol_read_payload_uint16(payload, read_idx);
                    *write_ptr++ = (current_sample & sample_bit_mask) * norm_factor;
                    read_idx += 2;
                    --total_samples;
                }

                /* send Calibration data array to callback */
                total_samples = num_of_bytes >> 1;
                callback_adc_calibration_data(context_adc_calibration_data,
                                          protocol_handle, endpoint,
                                          sample_data, total_samples);
                free(sample_data);
            }
        }
        return 1;
    }
    return 0;
}

//--------------------------------------------------------------------------------

static int32_t parse_algo_calibration_data(int32_t protocol_handle,
                        uint8_t endpoint, const uint8_t* payload,
                        uint16_t payload_size)
{
    if (((protocol_read_payload_uint8(payload, 0) == MSG_GET_ALGO_FLASH_CALIBRATION) || 
		(protocol_read_payload_uint8( payload, 0) == MSG_GET_ALGO_SRAM_CALIBRATION)) && 
		(payload_size == 5))
    {
        if (callback_algo_calibration_data)
        {
            Algo_Calibrations_t algo_cal_data;
			
			algo_cal_data.distance_offset_cm = protocol_read_payload_uint16(payload, 1);
			algo_cal_data.angle_offset_deg = protocol_read_payload_int16(payload, 3);

            callback_algo_calibration_data(context_algo_calibration_data,
                                          protocol_handle, endpoint,
                                          &algo_cal_data);
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

int32_t ep_calibration_is_compatible_endpoint(int32_t protocol_handle,
											uint8_t endpoint)
{
	return protocol_is_endpoint_compatible(protocol_handle, endpoint,
		&ep_calibration_definition);
}

void ep_calibration_set_callback_adc_calibration_data(Callback_ADC_Calibration_Data_t callback,
											void* context)
{
    callback_adc_calibration_data = callback;
    context_adc_calibration_data = context;
}

void ep_calibration_set_callback_algo_calibration_data(Callback_Algo_Calibration_Data_t callback,
											void* context)
{
    callback_algo_calibration_data = callback;
    context_algo_calibration_data = context;
}

//=====================================================================================

int32_t ep_calibration_set_adc_flash_calibration_data(int32_t protocol_handle,
											uint8_t endpoint)
{
    /* build message to send */
    uint8_t cmd_message[1];

    protocol_write_payload_uint8(cmd_message, 0, MSG_SET_ADC_FLASH_CALIBRATION);

    /* send message and process received response */
    return protocol_send_and_receive(protocol_handle, endpoint,
                                     &ep_calibration_definition,
                                     cmd_message, sizeof(cmd_message));
}


int32_t ep_calibration_set_adc_sram_calibration_data(int32_t protocol_handle,
											uint8_t endpoint)
{
    /* build message to send */
    uint8_t cmd_message[1];

    protocol_write_payload_uint8(cmd_message, 0, MSG_SET_ADC_SRAM_CALIBRATION);

    /* send message and process received response */
    return protocol_send_and_receive(protocol_handle, endpoint,
                                     &ep_calibration_definition,
                                     cmd_message, sizeof(cmd_message));
}

//--------------------------------------------------------------------------------

int32_t ep_calibration_get_adc_flash_calibration_data(int32_t protocol_handle,
                                            uint8_t endpoint)
{
    /* build message to send */
    uint8_t cmd_message[1];

    protocol_write_payload_uint8 (cmd_message, 0, MSG_GET_ADC_FLASH_CALIBRATION);

    /* send message and process received response */
    return protocol_send_and_receive(protocol_handle, endpoint,
                                     &ep_calibration_definition,
                                     cmd_message, sizeof(cmd_message));
}

int32_t ep_calibration_get_adc_sram_calibration_data(int32_t protocol_handle,
                                            uint8_t endpoint)
{
    /* build message to send */
    uint8_t cmd_message[1];

    protocol_write_payload_uint8 (cmd_message, 0, MSG_GET_ADC_SRAM_CALIBRATION);

    /* send message and process received response */
    return protocol_send_and_receive(protocol_handle, endpoint,
                                     &ep_calibration_definition,
                                     cmd_message, sizeof(cmd_message));
}

//--------------------------------------------------------------------------------

int32_t ep_calibration_clear_adc_flash_calibration_data(int32_t protocol_handle,
                                            uint8_t endpoint)
{
    /* build message to send */
    uint8_t cmd_message[1];

    protocol_write_payload_uint8(cmd_message, 0, MSG_CLEAR_ADC_FLASH_CALIBRATION);

    /* send message and process received response */
    return protocol_send_and_receive(protocol_handle, endpoint,
                                     &ep_calibration_definition,
                                     cmd_message, sizeof(cmd_message));
}

int32_t ep_calibration_clear_adc_sram_calibration_data(int32_t protocol_handle,
                                            uint8_t endpoint)
{
    /* build message to send */
    uint8_t cmd_message[1];

    protocol_write_payload_uint8(cmd_message, 0, MSG_CLEAR_ADC_SRAM_CALIBRATION);

    /* send message and process received response */
    return protocol_send_and_receive(protocol_handle, endpoint,
                                     &ep_calibration_definition,
                                     cmd_message, sizeof(cmd_message));
}

//======================================================================================

int32_t ep_calibration_set_algo_flash_calibration_data(int32_t protocol_handle,
											uint8_t endpoint,
											const Algo_Calibrations_t* algo_cal_data)
{
    /* build message to send */
    uint8_t cmd_message[5];

    protocol_write_payload_uint8 (cmd_message, 0, MSG_SET_ALGO_FLASH_CALIBRATION);
	protocol_write_payload_uint16(cmd_message, 1, algo_cal_data->distance_offset_cm);
	protocol_write_payload_int16 (cmd_message, 3, algo_cal_data->angle_offset_deg);
	
    /* send message and process received response */
    return protocol_send_and_receive(protocol_handle, endpoint,
                                     &ep_calibration_definition,
                                     cmd_message, sizeof(cmd_message));
}


int32_t ep_calibration_set_algo_sram_calibration_data(int32_t protocol_handle,
											uint8_t endpoint,
											const Algo_Calibrations_t* algo_cal_data)
{
    /* build message to send */
    uint8_t cmd_message[5];

    protocol_write_payload_uint8 (cmd_message, 0, MSG_SET_ALGO_SRAM_CALIBRATION);
	protocol_write_payload_uint16(cmd_message, 1, algo_cal_data->distance_offset_cm);
	protocol_write_payload_int16 (cmd_message, 3, algo_cal_data->angle_offset_deg);

    /* send message and process received response */
    return protocol_send_and_receive(protocol_handle, endpoint,
                                     &ep_calibration_definition,
                                     cmd_message, sizeof(cmd_message));
}

//--------------------------------------------------------------------------------

int32_t ep_calibration_get_algo_flash_calibration_data(int32_t protocol_handle,
                                            uint8_t endpoint)
{
    /* build message to send */
    uint8_t cmd_message[1];

    protocol_write_payload_uint8 (cmd_message, 0, MSG_GET_ALGO_FLASH_CALIBRATION);

    /* send message and process received response */
    return protocol_send_and_receive(protocol_handle, endpoint,
                                     &ep_calibration_definition,
                                     cmd_message, sizeof(cmd_message));
}

int32_t ep_calibration_get_algo_sram_calibration_data(int32_t protocol_handle,
                                            uint8_t endpoint)
{
    /* build message to send */
    uint8_t cmd_message[1];

    protocol_write_payload_uint8 (cmd_message, 0, MSG_GET_ALGO_SRAM_CALIBRATION);

    /* send message and process received response */
    return protocol_send_and_receive(protocol_handle, endpoint,
                                     &ep_calibration_definition,
                                     cmd_message, sizeof(cmd_message));
}

//--------------------------------------------------------------------------------

int32_t ep_calibration_clear_algo_flash_calibration_data(int32_t protocol_handle,
                                            uint8_t endpoint)
{
    /* build message to send */
    uint8_t cmd_message[1];

    protocol_write_payload_uint8(cmd_message, 0, MSG_CLEAR_ALGO_FLASH_CALIBRATION);

    /* send message and process received response */
    return protocol_send_and_receive(protocol_handle, endpoint,
                                     &ep_calibration_definition,
                                     cmd_message, sizeof(cmd_message));
}

int32_t ep_calibration_clear_algo_sram_calibration_data(int32_t protocol_handle,
                                            uint8_t endpoint)
{
    /* build message to send */
    uint8_t cmd_message[1];

    protocol_write_payload_uint8(cmd_message, 0, MSG_CLEAR_ALGO_SRAM_CALIBRATION);

    /* send message and process received response */
    return protocol_send_and_receive(protocol_handle, endpoint,
                                     &ep_calibration_definition,
                                     cmd_message, sizeof(cmd_message));
}

/* --- End of File -------------------------------------------------------- */
