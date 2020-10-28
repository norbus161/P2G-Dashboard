/**
* \file Protocol.c
*
* \brief This file implements the access Infineon sensor devices.
*
* The API to this module is split up into the two header files
* \ref Protocol.h and \ref Protocol_internal.h. See \ref Protocol.h for a
* basic description of the communication scheme. That header declares
* functions to be called by users of the communication library. The header
* \ref Protocol_internal.h declares those functions that are meant to be
* called by endpoint implementations inside the communication library.
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
#include "Protocol.h"
#define __PROTOCOL_INCLUDE_ENDPOINT_ONLY_API__
#include "Protocol_internal.h"
#undef __PROTOCOL_INCLUDE_ENDPOINT_ONLY_API__
#include "COMPort.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/*
==============================================================================
   2. LOCAL DEFINITIONS
==============================================================================
*/

/**
 * \internal
 * \defgroup InternalConstants
 *
 * \brief Constants used to define the message format.
 *
 * The messages exchanged between host and device contain some of those
 * constants to define the limits of a message in the byte stream.
 *
 * @{
 */
#define CNST_STARTBYTE_DATA           0x5A    /**< A pad load message begins
                                                   with this code byte. */
#define CNST_STARTBYTE_STATUS         0x5B    /**< A status message begins
                                                   with this code byte. */
#define CNST_END_OF_PAYLOAD           0xE0DB  /**< Every payload message ends
                                                   with this 16 bit value
                                                   (transmitted with low byte
                                                   first!). */

#define CNST_MSG_QUERY_ENDPOINT_INFO  0x00    /**< This command code sent to
                                                   endpoint 0 tells the device
                                                   to send information about
                                                   the protocol endpoints. */
#define CNST_MSG_ENDPOINT_INFO        0x00    /**< A message containing
                                                   endpoint information sent
                                                   from endpoint 0 starts with
                                                   this message type code. */
#define CNST_MSG_QUERY_FW_INFO        0x01    /**< This command code sent to
                                                   endpoint 0 tells the device
                                                   to send information about
                                                   its firmware. */
#define CNST_MSG_FW_INFO              0x01    /**< A message containing
                                                   firmware information sent
                                                   from endpoint 0 starts with
                                                   this message type code. */
#define CNST_MSG_FIRMWARE_RESET       0x02    /**< This command code sent to
                                                   endpoint 0 tells the device
                                                   to do a firmware reset. */

#define CNST_PROTOCOL_RECEIVED_PAYLOAD_MSG 0x01000000
                                              /**< If get_message received a
                                                   regular payload message,
                                                   this code is returned. */
/** @} */

/*
==============================================================================
   3. LOCAL TYPES
==============================================================================
*/

///@cond INTERNAL 

typedef struct
{
    uint32_t                     type;     /**< The type of the endpoint. */
    uint16_t                     version;  /**< The version of the endpoint
                                                implementation. */
    const Endpoint_Definition_t* endpoint_definiton;
                                           /**< A pointer to a structure
                                                containing information about
                                                the endpoint implementation on
                                                the host side. */
} Endpoint_t;

///@endcond

///@cond INTERNAL

/**
 * \internal 
 * \brief This structure holds information about an open connection to a
 *        device.
 *
 * For each open connection an instance of this structure is created and
 * stored in an internal list (see \ref handles). A valid protocol handle
 * identifies a such a structure in that list. For a valid connection
 * num_endpoints is always non zero.
 *
 * When the connection is established endpoints is setup with an array holding
 * an info structure for each endpoint present in the device.
 */
typedef struct
{
    int32_t     com_port_handle;  /**< \internal A handle to an open COM port. */
    uint8_t     num_endpoints;    /**< \internal The number of endpoints present in the
                                       connected device. */
    Endpoint_t* endpoints;        /**< \internal An array containing information
                                       about each endpoint present in the
                                       connected device. */
} Instance_t;

///@endcond

///@cond INTERNAL
/**
 * \internal
 * \brief This structure contains all information from a received payload
 *        message.
 *
 * An instance of this structure must be passed to \ref get_message and if a
 * payload message is received, the information is placed in that instance.
 */
typedef struct
{
    uint8_t  endpoint;      /**< The number of the endpoint that sent the
                                 payload message. */
    uint8_t* payload;       /**< A pointer to the buffer containing the
                                 payload. */
    uint16_t payload_size;  /**< The size of the payload in bytes. */
} Message_Info_t;

///@endcond

/*
==============================================================================
   4. DATA
==============================================================================
*/

/**
 * \internal
 * \defgroup HandleManagement
 *
 * \brief This static data is used to manage the protocol handles returned to
 *        the user.
 *
 * When \ref protocol_connect is called a handle is returned to the user. To
 * make the library robust against being used with invalid pointers, the
 * handle is not just a pointer to an internal data structure. Instead the
 * handle is an integer number that is used as an index to an internal array
 * of \ref Instance_t structures. That allows to check the index and return an
 * error code when an invalid handle is provided by the user.
 *
 * For book keeping the internal array handles is accompanied by two counters
 * for the capacity of the handles and open connections. When a new connection
 * is opened the both counters are equal, array handles is extended, to take
 * another handle. On the other hand, the array is not shrinked when a
 * connection is closed. The only exception is the case when the number of
 * open connections decreases to 0. In this case the array is deleted
 * completely to avoid memory leaks.
 *
 * @{
 */

static Instance_t* handles = NULL;        /**< An array of handles to open
                                               connections. */
static size_t num_allocated_handles = 0;  /**< The current size of the handle
                                               array. */
static size_t num_open_handles = 0;       /**< The current number of open
                                               handles. */

/** @} */

/* Include list of known endpoints (see Protocol_KnownEndpoints.h for details)
 */
#define __PROTOCOL_INCLUDES_ENDPOINT_LIST__
#include "Protocol_KnownEndpoints.h"
#undef __PROTOCOL_INCLUDES_ENDPOINT_LIST__

/**
 * \internal
 * \brief The number of known endpoints
 *
 * This macro is resolved to the size of entries in known_endpoints which is
 * defined in \ref Protocol_KnownEndpoints.h.
 */
#define NUM_KNOWN_ENDPOINTS \
    (sizeof(known_endpoints) / sizeof(const Endpoint_Definition_t*))

/*
==============================================================================
   5. LOCAL FUNCTION PROTOTYPES
==============================================================================
*/

/**
 * \internal
 * \brief This function tries to bring a byte stream from the COM port back
 * into a defined state.
 *
 * When an error occurs during message reception, the byte stream is in an
 * undefined state. To recover from that state and get in sync again. The
 * function flushes all received data until a receive timeout occurs at the
 * COM port.
 *
 * The assumption that after some time without transmission, the following
 * data will be a proper message again.
 *
 * \param[in] com_port_handle  A handle to an open COM port, the device is
 *                             connected to.
 */
static void recover_from_receive_error(int32_t com_port_handle);

/**
 * \internal
 * \brief This function sends a payload message to an endpoint of a connected
 *        device.
 *
 * This function builds a data package forming a payload message and sends
 * through the specified COM port.
 *
 * The data package contains a message header consisting of a message start
 * code the addressed endpoint and the size of the payload, followed by the
 * payload itself and finally an end-of-message code.
 *
 * \param[in] com_port_handle  A handle to an open COM port, the device is
 *                             connected to.
 * \param[in] endpoint         The number of the endpoint, the message is sent
 *                             to.
 * \param[in] payload          A pointer to the payload of the message.
 * \param[in] payload_size     The size of the payload in size.
 */
static void send_message(int32_t com_port_handle, uint8_t endpoint,
                         const uint8_t* payload, uint16_t payload_size);

/**
 * \internal
 * \brief This function reads the next message from the COM port
 *
 * The function reads data from the COM port and interprets it. If it is a
 * payload message it extracts the payload, and the endpoint number that sent
 * the message and writes all information to message_info. If the message is a
 * status message, the function extracts the status code and the endpoint that
 * sent it and returns it.
 *
 * If the received data is not a proper message, or no data is received at
 * all, an error code is returned.
 *
 * The caller must allocate an instance of \ref Message_Info_t and pass it to
 * the function. The instance may be uninitialized.
 *
 * \param[in]  com_port_handle  A handle to an open COM port, the device is
 *                              connected to.
 * \param[out] message_info     A pointer to a message info struct where a
 *                              received payload message will be stored to.
 *
 * \return If a payload message is received, all information contained in that
 *         message is written to message_info and
 *         \ref CNST_PROTOCOL_RECEIVED_PAYLOAD_MSG is returned. If a status
 *         message is received the function returns a positive number
 *         containing the 16 bit status code in the bits 0...15 and the
 *         endpoint that sent the code in bits 16...23. If an error occurred
 *         a negative error code is returned.
 */
static int32_t get_message(int32_t com_port_handle,
                           Message_Info_t* message_info);

						   
/*
==============================================================================
  6. LOCAL FUNCTIONS
==============================================================================
*/

///@cond INTERNAL 


///@endcond

static void recover_from_receive_error(int32_t com_port_handle)
{
    /* read until buffer is empty */
    uint8_t dummy_data[1024];
    size_t received_bytes = sizeof(dummy_data);

    while (received_bytes == sizeof(dummy_data))
    {
        received_bytes = com_get_data(com_port_handle,
                                      dummy_data, sizeof(dummy_data));
    }

    /* now we have run out of data, protocol should be in sync again */
}

static void send_message(int32_t com_port_handle, uint8_t endpoint,
                         const uint8_t* payload, uint16_t payload_size)
{
    /* setup message header and tail */
    uint8_t message_header[4];
    uint8_t message_tail[2];

    message_header[0] = CNST_STARTBYTE_DATA;
    message_header[1] = endpoint;
    message_header[2] = payload_size & 0xFF;
    message_header[3] = payload_size >> 8;

    message_tail[0] = CNST_END_OF_PAYLOAD & 0xFF;
    message_tail[1] = CNST_END_OF_PAYLOAD >> 8;

    /* send message */
    com_send_data(com_port_handle, message_header, 4);
    com_send_data(com_port_handle, payload, payload_size);
    com_send_data(com_port_handle, message_tail, 2);
}

static int32_t get_message(int32_t com_port_handle,
                           Message_Info_t* message_info)
{
    uint8_t message_header[4];
    size_t num_received_bytes;

    /* read message header */
    /* ------------------- */
    num_received_bytes = com_get_data(com_port_handle,
                                      message_header, sizeof(message_header));

    /*
     * If number of received bytes is smaller than expected, one reason could
     * be that the message came in shortly before the timeout. If this has
     * happened, the rest of the message bytes must be available now. Try
     * another read.
     */
    if ((num_received_bytes < sizeof(message_header)))
    {
        num_received_bytes += com_get_data(com_port_handle,
                                           message_header +
                                             num_received_bytes,
                                           sizeof(message_header) -
                                             num_received_bytes);
    }

    /* if no data at all has been received, everything is all right, but there
     * is no message
     */
    if (num_received_bytes  == 0)
    {
        return PROTOCOL_ERROR_RECEIVED_NO_MESSAGE;
    }

    /* if not enough bytes for a full message header have been received,
     * something went wrong
     */
    else if (num_received_bytes < sizeof(message_header))
    {
        recover_from_receive_error(com_port_handle);
        return PROTOCOL_ERROR_RECEIVED_TIMEOUT;
    }

    /* read rest of message */
    /* -------------------- */
    if (message_header[0] == CNST_STARTBYTE_DATA)
    {
        uint8_t* payload;
        uint16_t payload_size;
        uint8_t message_tail[2];

        /* allocate memory and receive payload */
        payload_size = (uint16_t)message_header[2] |
                      ((uint16_t)message_header[3]) << 8;
        payload = (uint8_t*)malloc(payload_size);

        num_received_bytes = com_get_data(com_port_handle,
                                          payload, payload_size);

        /* check if payload has been received completely */
        if (num_received_bytes < payload_size)
        {
            /* free payload memory, it's incomplete */
            free(payload);

            recover_from_receive_error(com_port_handle);

            return PROTOCOL_ERROR_RECEIVED_TIMEOUT;
        }

        /* check message tail */
        num_received_bytes = com_get_data(com_port_handle, &message_tail,
                                          sizeof(message_tail));

        if ((num_received_bytes != sizeof(message_tail)) ||
            (message_tail[0] != (CNST_END_OF_PAYLOAD & 0xFF)) ||
            (message_tail[1] != CNST_END_OF_PAYLOAD >> 8))
        {
            /* free payload memory, it's incomplete */
            free(payload);

            recover_from_receive_error(com_port_handle);

            return PROTOCOL_ERROR_RECEIVED_BAD_MESSAGE_END;
        }

        message_info->endpoint     = message_header[1];
        message_info->payload      = payload;
        message_info->payload_size = payload_size;

        return CNST_PROTOCOL_RECEIVED_PAYLOAD_MSG;
    }
    else if (message_header[0] == CNST_STARTBYTE_STATUS)
    {
        uint8_t endpoint     = message_header[1];
        uint16_t status_code = (uint16_t)message_header[2] |
                              ((uint16_t)message_header[3]) << 8;

        return ((int)endpoint << 16) | status_code;
    }
    else
    {
        recover_from_receive_error(com_port_handle);
        return PROTOCOL_ERROR_RECEIVED_BAD_MESSAGE_START;
    }
}

/*
==============================================================================
   7. EXPORTED FUNCTIONS
==============================================================================
*/

int32_t protocol_connect(const char* port_name)
{
    Instance_t protocol_instance;
    Message_Info_t message_info;
    int32_t receive_status;
    uint8_t i;
    int32_t new_handle;

    /* open the COM port */
    /* ----------------- */
    protocol_instance.com_port_handle = com_open(port_name);

    if (protocol_instance.com_port_handle < 0)
    {
        return PROTOCOL_ERROR_COULD_NOT_OPEN_COM_PORT;
    }

    /* get Endpoint information from device */
    /* ------------------------------------ */
    protocol_instance.num_endpoints = 0;
    protocol_instance.endpoints = NULL;

    /* send a message with command code to query endpoint info to endpoint 0
     */
    {
        uint8_t uQueryMessage[1] = { CNST_MSG_QUERY_ENDPOINT_INFO };
        send_message(protocol_instance.com_port_handle, 0,
                     uQueryMessage, sizeof(uQueryMessage));
    }

    /* read and parse reply message from connected device */
    /* -------------------------------------------------- */
    message_info.payload = NULL;
    receive_status = get_message(protocol_instance.com_port_handle,
                                 &message_info);

    if ((receive_status != CNST_PROTOCOL_RECEIVED_PAYLOAD_MSG) ||
        (message_info.endpoint != 0) ||
        (message_info.payload_size < 2) ||
        (message_info.payload[0] != CNST_MSG_ENDPOINT_INFO))
    {
        /* This is not the expected payload, clean up and quit. */
        free(message_info.payload);
        com_close(protocol_instance.com_port_handle);
        return PROTOCOL_ERROR_DEVICE_NOT_COMPATIBLE;
    }

    /* read number of endpoints and check message size */
    protocol_instance.num_endpoints = message_info.payload[1];

    if ((message_info.payload_size !=
           6 * protocol_instance.num_endpoints + 2) ||
        (protocol_instance.num_endpoints == 0))
    {
        free(message_info.payload);
        com_close(protocol_instance.com_port_handle);
        return PROTOCOL_ERROR_DEVICE_NOT_COMPATIBLE;
    }

    /* allocate array to hold endpoint information */
    protocol_instance.endpoints =
      (Endpoint_t*)malloc(protocol_instance.num_endpoints *
                          sizeof(Endpoint_t));

    /* iterate over all endpoint info records in the message */
    for (i = 0; i < protocol_instance.num_endpoints; ++i)
    {
        uint32_t j;
        Endpoint_t* endpoint = &protocol_instance.endpoints[i];

        /* read endpoint type and version from payload */
        endpoint->type    = protocol_read_payload_uint32(message_info.payload,
                                                         2 + i * 6);
        endpoint->version = protocol_read_payload_uint16(message_info.payload,
                                                         6 + i * 6);

        /* find endpoint implementation matching type and version */
        endpoint->endpoint_definiton = NULL;

        for (j = 0; j < NUM_KNOWN_ENDPOINTS; ++j)
        {
            if ((known_endpoints[j]->type == endpoint->type) &&
                (known_endpoints[j]->min_version <= endpoint->version) &&
                (known_endpoints[j]->max_version >= endpoint->version))
            {
                endpoint->endpoint_definiton = known_endpoints[j];
                break;
            }
        }
    }

    /* free payload memory */
    free(message_info.payload);

    /* consume the expected status message */
    receive_status = get_message(protocol_instance.com_port_handle,
                                 &message_info);

    if (receive_status != ((/*endpoint*/0 << 16) | /*status code*/0x0000))
    {
        /*
         * If the next message was not a status message with Code 0, something
         * went wrong, so remove all the endpoints, close interface and
         * return.
         */
        if (receive_status == CNST_PROTOCOL_RECEIVED_PAYLOAD_MSG)
        {
            free(message_info.payload);
        }

        free(protocol_instance.endpoints);
        com_close(protocol_instance.com_port_handle);
        return PROTOCOL_ERROR_DEVICE_NOT_COMPATIBLE;
    }

    /* add the handle to the array of open COM ports */
    /* --------------------------------------------- */

    /* increase capacity of handle array if needed */
    if (num_open_handles == num_allocated_handles)
    {
        Instance_t* pNewHandles = (Instance_t*)malloc((num_open_handles + 1) *
                                                      sizeof(Instance_t));

        if (num_allocated_handles > 0)
        {
            memcpy(pNewHandles, handles,
                   sizeof(Instance_t) * num_allocated_handles);
        }

        pNewHandles[num_allocated_handles++].num_endpoints = 0;

        free(handles);
        handles = pNewHandles;
    }

    /* add new handle to table */
    new_handle = 0;

    while (handles[new_handle].num_endpoints != 0)
    {
        ++new_handle;
    }

    handles[new_handle] = protocol_instance;
    ++num_open_handles;

    return new_handle;
}

void protocol_disconnect(int32_t protocol_handle)
{
    /* check if handle is valid */
    if ((protocol_handle >= 0) &&
        (protocol_handle < (int32_t)num_allocated_handles) &&
        (handles[protocol_handle].num_endpoints != 0))
    {
        /* close COM port */
        com_close(handles[protocol_handle].com_port_handle);
        handles[protocol_handle].com_port_handle = -1;

        /* free memory of endpoint table */
        free (handles[protocol_handle].endpoints);
        handles[protocol_handle].endpoints = NULL;

        /* remove handle from the table */
        handles[protocol_handle].num_endpoints = 0;
        --num_open_handles;

        /* if all handles have been closed, free handle table to prevent
         * memory leaks
         */
        if (num_open_handles == 0)
        {
            num_allocated_handles = 0;
            free(handles);
            handles = NULL;
        }
    }
}

int32_t protocol_get_num_endpoints(int32_t protocol_handle)
{
    /* check if handle is valid */
    if ((protocol_handle < 0) ||
        (protocol_handle >= (int32_t)num_allocated_handles) ||
        (handles[protocol_handle].num_endpoints == 0))
    {
        return PROTOCOL_ERROR_INVALID_HANDLE;
    }

    return handles[protocol_handle].num_endpoints;
}

int32_t protocol_get_firmware_information(int32_t protocol_handle,
                                          Firmware_Information_t* information)
{
    Instance_t* protocol;
    Message_Info_t message_info;
    int32_t receive_status;

    /* check if handle is valid */
    if ((protocol_handle < 0) ||
        (protocol_handle >= (int32_t)num_allocated_handles) ||
        (handles[protocol_handle].num_endpoints == 0))
    {
        return PROTOCOL_ERROR_INVALID_HANDLE;
    }

    protocol = &handles[protocol_handle];

    /* send a message with command code to query endpoint info to endpoint 0
     */
    {
        uint8_t uQueryMessage[1] = { CNST_MSG_QUERY_FW_INFO };
        send_message(protocol->com_port_handle, 0,
                     uQueryMessage, sizeof(uQueryMessage));
    }

    /* read and parse reply message from connected device */
    message_info.payload = NULL;
    receive_status = get_message(protocol->com_port_handle, &message_info);

    if ((receive_status != CNST_PROTOCOL_RECEIVED_PAYLOAD_MSG) ||
        (message_info.endpoint != 0) ||
        (message_info.payload_size < 7) ||
        (message_info.payload[0] != CNST_MSG_FW_INFO))
    {
        /* This is not the expected payload, clean up and quit. */
        free(message_info.payload);
        return PROTOCOL_ERROR_DEVICE_NOT_COMPATIBLE;
    }

    information->description = NULL;
    information->version_major =
                         protocol_read_payload_uint16(message_info.payload, 1);
    information->version_minor =
                         protocol_read_payload_uint16(message_info.payload, 3);
    information->version_build =
                         protocol_read_payload_uint16(message_info.payload, 5);

    if (message_info.payload_size > 7)
    {
        uint16_t description_size = message_info.payload_size - 7;
        information->description = malloc(description_size);
        memcpy(information->description, message_info.payload + 7,
               description_size);
    }

    /* free payload memory */
    free(message_info.payload);

    /* consume the expected status message */
    receive_status = get_message(protocol->com_port_handle, &message_info);

    if (receive_status != ((/*endpoint*/0 << 16) | /*status code*/0x0000))
    {
        /*
         * If the next message was not a status message with Code 0, something
         * went wrong, so remove all the endpoints, close interface and
         * return.
         */
        if (receive_status == CNST_PROTOCOL_RECEIVED_PAYLOAD_MSG)
        {
            free(message_info.payload);
        }

        return PROTOCOL_ERROR_DEVICE_NOT_COMPATIBLE;
    }

    return 0;
}

int32_t protocol_do_firmware_reset(int32_t protocol_handle)
{
    Instance_t* protocol;
    Message_Info_t message_info;
    int32_t receive_status;

    /* check if handle is valid */
    if ((protocol_handle < 0) ||
        (protocol_handle >= (int32_t)num_allocated_handles) ||
        (handles[protocol_handle].num_endpoints == 0))
    {
        return PROTOCOL_ERROR_INVALID_HANDLE;
    }

    protocol = &handles[protocol_handle];

    /* send a message with command code to query endpoint info to endpoint 0
     */
    {
        uint8_t uQueryMessage[1] = { CNST_MSG_FIRMWARE_RESET };
        send_message(protocol->com_port_handle, 0,
                     uQueryMessage, sizeof(uQueryMessage));
    }

    /* consume the expected status message */
    receive_status = get_message(protocol->com_port_handle, &message_info);

    if (receive_status != ((/*endpoint*/0 << 16) | /*status code*/0x0000))
    {
        /*
         * If the next message was not a status message with Code 0, something
         * went wrong, so remove all the endpoints, close interface and
         * return.
         */
        if (receive_status == CNST_PROTOCOL_RECEIVED_PAYLOAD_MSG)
        {
            free(message_info.payload);
        }

        return PROTOCOL_ERROR_DEVICE_NOT_COMPATIBLE;
    }

    return 0;
}

int32_t protocol_get_endpoint_info(int32_t protocol_handle, uint8_t endpoint,
                                   Endpoint_Info_t* endpoint_info)
{
    Endpoint_t* requested_endpoint;

    /* check if handle is valid */
    if ((protocol_handle < 0) ||
        (protocol_handle >= (int32_t)num_allocated_handles) ||
        (handles[protocol_handle].num_endpoints == 0))
    {
        return PROTOCOL_ERROR_INVALID_HANDLE;
    }

    /* check if endpoint exists */
    if ((endpoint == 0) ||
        (endpoint > handles[protocol_handle].num_endpoints))
    {
        return PROTOCOL_ERROR_ENDPOINT_DOES_NOT_EXIST;
    }

    /* write endpoint information to endpoint_info */
    requested_endpoint = &handles[protocol_handle].endpoints[endpoint - 1];

    endpoint_info->type        = requested_endpoint->type;
    endpoint_info->version     = requested_endpoint->version;
    endpoint_info->description = "";

    /* get endpoint description, if it is a known endpoint type */
    if (requested_endpoint->endpoint_definiton)
    {
        endpoint_info->description =
          requested_endpoint->endpoint_definiton->description;
    }

    return 0;
}

int32_t protocol_is_endpoint_compatible(int32_t protocol_handle,
                                        uint8_t endpoint,
                                        const Endpoint_Definition_t*
                                          endpoint_definiton)
{
    Endpoint_Info_t sEndpointInfo;

    if (protocol_get_endpoint_info(protocol_handle, endpoint, &sEndpointInfo))
    {
        return PROTOCOL_ERROR_ENDPOINT_DOES_NOT_EXIST;
    }

    if (sEndpointInfo.type != endpoint_definiton->type)
    {
        return PROTOCOL_ERROR_ENDPOINT_WRONG_TYPE;
    }

    if (sEndpointInfo.version < endpoint_definiton->min_version)
    {
        return PROTOCOL_ERROR_ENDPOINT_VERSION_TOO_OLD;
    }

    if (sEndpointInfo.version > endpoint_definiton->max_version)
    {
        return PROTOCOL_ERROR_ENDPOINT_VERSION_TOO_NEW;
    }

    return 0;
}

int32_t protocol_send_and_receive(int32_t protocol_handle,
                                  uint8_t endpoint,
                                  const Endpoint_Definition_t*
                                    endpoint_definiton,
                                  const uint8_t* payload,
                                  uint16_t payload_size)
{
    Instance_t* protocol;
    Message_Info_t message_info;
    int32_t status_code;

    /* check handle and endpoint compatibility */
    /* --------------------------------------- */
    /* check handle */
    if ((protocol_handle < 0) ||
        (protocol_handle >= (int32_t)num_allocated_handles) ||
        (handles[protocol_handle].num_endpoints == 0))
    {
        return PROTOCOL_ERROR_INVALID_HANDLE;
    }

    protocol = &handles[protocol_handle];

    /* check if endpoint exists */
    if ((endpoint == 0) || (endpoint > protocol->num_endpoints))
    {
        return PROTOCOL_ERROR_ENDPOINT_DOES_NOT_EXIST;
    }

    /* endpoint has correct type and version */
    if (protocol->endpoints[endpoint-1].endpoint_definiton !=
          endpoint_definiton)
    {
        return protocol_is_endpoint_compatible(protocol_handle, endpoint,
                                               endpoint_definiton);
    }

    /* send message */
    send_message(protocol->com_port_handle, endpoint, payload, payload_size);

    /* receive messages from the board */
    while ((status_code = get_message(protocol->com_port_handle,
                                      &message_info)) ==
             CNST_PROTOCOL_RECEIVED_PAYLOAD_MSG)
    {
        /* forward message to endpoint implementation */
        if (message_info.endpoint > 0)
        {
            const Endpoint_Definition_t* endpoint_ptr =
              protocol->endpoints[message_info.endpoint-1].endpoint_definiton;

            if (endpoint_ptr)
            {
                endpoint_ptr->parse_payload(protocol_handle,
                                            message_info.endpoint,
                                            message_info.payload,
                                            message_info.payload_size);
            }
        }

        free(message_info.payload);
    }

    return status_code;
}


const char* protocol_get_status_code_description(int32_t protocol_handle,
                                                 int32_t status_code)
{
    // first check status code that are common to all endpoints
    if ((status_code & 0xFFFF) == PROTOCOL_STATUS_OK)
    {
        return "No Error.";
    }
    else if (status_code == PROTOCOL_STATUS_DEVICE_BAD_COMMAND)
    {
        return "The device received a message at endpoint 0 that could not be understood.";
    }

    if (status_code >= 0)
    {
        // if status code is positive, it's a status code sent from the device
        // -------------------------------------------------------------------
        uint8_t endpoint = (uint8_t)(status_code >> 16);
        uint16_t error_code = status_code & 0xFFFF;

        // endpoint 0 is the protocol itself, so handle error code right here
        if (endpoint == 0)
        {
            switch (error_code)
            {
                case PROTOCOL_STATUS_DEVICE_TIMEOUT:
                    return "A time out occured during message transmission from host to device.";

                case PROTOCOL_STATUS_DEVICE_BAD_MESSAGE_START:
                    return "The device received a message with a bad start sequence.";

                case PROTOCOL_STATUS_DEVICE_BAD_ENDPOINT_ID:
                    return "The device received a message for a non existing endpoint.";

                case PROTOCOL_STATUS_DEVICE_NO_PAYLOAD:
                    return "The device received a data message without payload.";

                case PROTOCOL_STATUS_DEVICE_OUT_OF_MEMORY:
                    return "The device's memory is not sufficient to process the receive data message.";

                case PROTOCOL_STATUS_DEVICE_BAD_PAYLOAD_END:
                    return "The device received a message with a bad end sequence.";

                default:
                    return "Unknown Error";
            }
        }
        else
        {
            Endpoint_t* requested_endpoint;

            /* check if handle is valid, before accessing the endpoints */
            if ((protocol_handle < 0) ||
                (protocol_handle >= (int32_t)num_allocated_handles) ||
                (handles[protocol_handle].num_endpoints == 0))
            {
                return "Invalid Handle.";
            }

            /* check if endpoint exists and get a short cut */
            if (endpoint > handles[protocol_handle].num_endpoints)
            {
                return "Invalid Endpoint.";
            }

            requested_endpoint =
              &(handles[protocol_handle].endpoints[endpoint-1]);

            /* get error message from endpoint implementation */
            if (requested_endpoint->endpoint_definiton)
            {
                return requested_endpoint->endpoint_definiton->
                         get_status_descr(error_code);
            }

            return "Unknown Error";
        }
    }
    else
    {
        // if status code is negatve, an communication error has occurred
        // --------------------------------------------------------------

        switch (status_code)
        {
            case PROTOCOL_ERROR_INVALID_HANDLE:
                return "The protocol handle is not a valid.";

            case PROTOCOL_ERROR_COULD_NOT_OPEN_COM_PORT:
                return "The specified COM port could not be opened.";

            case PROTOCOL_ERROR_DEVICE_NOT_COMPATIBLE:
                return "The device at the specified COM port is not compatible with the protocol.";

            case PROTOCOL_ERROR_RECEIVED_NO_MESSAGE:
                return "The device didn't send any message.";

            case PROTOCOL_ERROR_RECEIVED_TIMEOUT:
                return "A timeout occurred while receiving a message from the device.";

            case PROTOCOL_ERROR_RECEIVED_BAD_MESSAGE_START:
                return "The device sent a message with a bad start sequence.";

            case PROTOCOL_ERROR_RECEIVED_BAD_MESSAGE_END:
                return "The device sent a message with a bad end sequence.";

            case PROTOCOL_ERROR_ENDPOINT_DOES_NOT_EXIST:
                return "The requested endpoint does not exist.";

            case PROTOCOL_ERROR_ENDPOINT_WRONG_TYPE:
                return "The requested endpoint is not of the type checked for.";

            case PROTOCOL_ERROR_ENDPOINT_VERSION_TOO_OLD:
                return "The requested endpoint's version is too old to be supported.";

            case PROTOCOL_ERROR_ENDPOINT_VERSION_TOO_NEW:
                return "The requested endpoint's version is too new to be supported.";

            default:
                return "Unknown Error";
        }
    }
}

/* --- End of File -------------------------------------------------------- */
