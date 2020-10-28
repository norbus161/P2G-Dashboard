/**
 * \file Protocol.h
 *
 * \brief This file defines the basic API to access Infineon sensor devices.
 *
 * The communication protocol is a generic protocol to exchange messages with
 * microcontroller-based devices over a generic byte stream connection
 * (typically a virtual serial port over USB). The communication is always
 * initiated by the host, while the connected microcontroller device responds
 * to messages received from the host.
 *
 * The protocol defines two types of messages, payload messages and status
 * messages. A payload message contains a block of data with arbitrary size
 * and meaning. A status message contains a 16 bit status code. Messages from
 * the host to the device are always payload messages. Whenever the device
 * receives a payload message from the host, it responds with one (and only
 * one) status message. The device may also send an arbitrary number of
 * additional payload messages preceding that status message. Once the device
 * has sent the status message, it won't send any more messages until it
 * receives a new message from the host.
 *
 * Each message from the host to the device is addressed to a certain logical
 * endpoint and each message from the device to the host is sent from a
 * logical endpoint. The number of logical endpoints is defined by the device.
 * Endpoints are continuously enumerated starting with 1. At least one logical
 * endpoint is always present. Endpoints are used to define functional groups
 * in the device.
 *
 * Each logical endpoint is of a defined type, which defines the meaning of
 * the message payload exchanged with that endpoint. Additionally each
 * endpoint has a version number that allows to distinguish between slight
 * modifications of the endpoints set of known messages. The host can query
 * type and version of the endpoints present in the device.
 *
 * The API functions defined in this header file just allow basic access to
 * the device, such as connecting, disconnecting and query endpoint
 * information. The module \ref Protocol.c further also implements functions
 * for message exchange, but these functions are not meant to be used
 * directly. Instead each type of endpoint should have a separate module that
 * implements function to exchange messages tailored to the functionality of
 * that endpoint. Those endpoint implementations may use the communication
 * functions implemented in the module Protocol.c and declared in the header
 * \ref Protocol_internal.h.
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

#ifndef PROTOCOL_H_INCLUDED
#define PROTOCOL_H_INCLUDED

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/
#include <stdint.h>

/* Enable C linkage if header is included in C++ files */
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*
==============================================================================
   2. DEFINITIONS
==============================================================================
*/

/**
 * \defgroup StatusCodes StatusCodes
 *
 * \brief General protocol status codes.
 *
 * Whenever the connected device receives a message with improper format it
 * will reply with one of these status codes. The status codes will always be
 * sent from endpoint 0.
 *
 * @{
 */
#define PROTOCOL_STATUS_OK                       0x0000 /**< No error
                                                             occurred. */
#define PROTOCOL_STATUS_DEVICE_TIMEOUT           0x0001 /**< The message
                                                             seemed to end,
                                                             but more data was
                                                             expected. */
#define PROTOCOL_STATUS_DEVICE_BAD_MESSAGE_START 0x0002 /**< The start byte
                                                             of the message
                                                             was incorrect. */
#define PROTOCOL_STATUS_DEVICE_BAD_ENDPOINT_ID   0x0003 /**< An endpoint with
                                                             the requested ID
                                                             is not present.
                                                             */
#define PROTOCOL_STATUS_DEVICE_NO_PAYLOAD        0x0005 /**< A message with no
                                                             payload is not
                                                             supported. */
#define PROTOCOL_STATUS_DEVICE_OUT_OF_MEMORY     0x0006 /**< There is not
                                                             enough memory to
                                                             store the
                                                             payload. */
#define PROTOCOL_STATUS_DEVICE_BAD_PAYLOAD_END   0x0007 /**< The message did
                                                             not end with the
                                                             expected
                                                             sequence. */
#define PROTOCOL_STATUS_DEVICE_BAD_COMMAND       0xFFFF /**< The command sent
                                                             to endpoint 0 is
                                                             not defined. */
/** @} */

/**
 * \defgroup ErrorCodes ErrorCodes
 *
 * \brief Protocol status codes.
 *
 * If an error occurs during usage of the protocol or endpoint functions, one
 * of the error codes defined here is returned. The error codes can be grouped
 * in 4 classes.
 * 
 * 1) General errors
 * 2) Errors that occur when a connection can't be established.
 * 3) Errors that occur when unexpected (or no) data is received from the
 *    connected device
 * 4) Errors that occur due to improper endpoint request.
 *
 * @{
 */

/* Theese are the general errors. */
#define PROTOCOL_ERROR_INVALID_HANDLE                -1 /**< The given
                                                             protocol handle
                                                             is not a valid
                                                             handle to an open
                                                             protocol
                                                             connection. */

/* These error codes are returned when the connection could not be
 * established.
 */
#define PROTOCOL_ERROR_COULD_NOT_OPEN_COM_PORT     -100 /**< The specified COM
                                                             port could not be
                                                             opened. */
#define PROTOCOL_ERROR_DEVICE_NOT_COMPATIBLE       -101 /**< The device at the
                                                             specified COM
                                                             port is not
                                                             compatible with
                                                             the protocol. */

/* These error codes are returned when an error occurs during message
 * reception.
 */
#define PROTOCOL_ERROR_RECEIVED_NO_MESSAGE        -1000 /**< No message was
                                                             received from the
                                                             device at all. */
#define PROTOCOL_ERROR_RECEIVED_TIMEOUT           -1001 /**< The message was
                                                             not received
                                                             completely in the
                                                             expected
                                                             interval. */
#define PROTOCOL_ERROR_RECEIVED_BAD_MESSAGE_START -1002 /**< The start byte of
                                                             the received
                                                             message was
                                                             incorrect. */
#define PROTOCOL_ERROR_RECEIVED_BAD_MESSAGE_END   -1003 /**< The received
                                                             message did not
                                                             end with the
                                                             expected
                                                             sequence. */

/* These error codes are returned when a wrong endpoint is requested. */
#define PROTOCOL_ERROR_ENDPOINT_DOES_NOT_EXIST    -2000 /**< The requested
                                                             endpoint does not
                                                             exist.*/
#define PROTOCOL_ERROR_ENDPOINT_WRONG_TYPE        -2001 /**< The requested
                                                             endpoint is not
                                                             of a compatible
                                                             type. */
#define PROTOCOL_ERROR_ENDPOINT_VERSION_TOO_OLD   -2002 /**< The requested
                                                             endpoint's
                                                             version is too
                                                             old to be
                                                             supported. */
#define PROTOCOL_ERROR_ENDPOINT_VERSION_TOO_NEW   -2003 /**< The requested
                                                             endpoint's
                                                             version is too
                                                             new to be
                                                             supported. */

/** @} */

/*
==============================================================================
   3. TYPES
==============================================================================
*/

/**
 * \brief This structure contains information about the firmware.
 *
 * The protocol expects an instance of this structure to be externally defined
 * in the firmware project.
 * \code
 * Firmware_Information_t firmware_information;
 * \endcode
 * The information in firmware_information will be sent to the host on
 * request.
 */
typedef struct
{
    char*    description;   /**< A pointer to a zero-terminated string
                                 containing a firmware description. */
    uint16_t version_major; /**< The firmware version major number. */
    uint16_t version_minor; /**< The firmware version minor number. */
    uint16_t version_build; /**< The firmware version build number. */
} Firmware_Information_t;


/**
 * \brief This structure contains information about an endpoint.
 *
 * For each endpoint present in the connected device, the function
 * \ref protocol_get_endpoint_info can be used to retrieve information about.
 * This structure will be used to return the information to the user.
 */
typedef struct
{
    uint32_t    type;         /**< This is an unique code that identifies the
                                   type of the endpoint. */
    uint16_t    version;      /**< This is the version of the endpoint
                                   implementation on the connected device. */
    const char* description;  /**< This is a human readable zero-terminated
                                   string describing the  endpoint type. The
                                   memory of this string must not be freed or
                                   modified by the caller. */
} Endpoint_Info_t;

/*
==============================================================================
   5. FUNCTION PROTOTYPES AND INLINE FUNCTIONS
==============================================================================
*/

/**
 * \brief This function establishes a connection to a device through a COM
 *        port
 *
 * This function tries to open the specified COM port. The COM port name must
 * identify an existing COM port and is platform and machine specific. It is
 * recommended to call \ref com_get_port_list to get a list of available COM
 * ports. Port names not in the returned list won't be accepted.
 *
 * If the specified COM could be opened, information about the endpoints
 * present in the connected device is queried. If endpoint information could
 * not be retrieved properly, the device is considered to not be compatible
 * with the communication protocol and the connection is closed again.
 *
 * If a connection can be established, the function returns a handle that must
 * be passed to all following function calls to identify the device. When the
 * connection is not needed any more, it should be closed by calling
 * \ref protocol_disconnect.
 *
 * \param[in] port_name  A zero-terminated string containing the device name
 *                       of the COM port to use.
 *
 * \return If a connection could be established the function returns a handle
 *         that is positive or 0. If the connection could not be established,
 *         a negative error code is returned.
 */
int32_t protocol_connect(const char* port_name);

/**
 * \brief This function closes an open connection.
 *
 * If a valid handle to an open connection is provided, the according
 * connection is closed and the handle is marked as invalid. If the handle is
 * invalid, nothing will happen. Either way the handle must not be used any
 * more after calling this function.
 *
 * \param[in] protocol_handle  A handle to an open connection.
 */
void protocol_disconnect(int32_t protocol_handle);

/**
 * \brief This function returns the number of endpoints in the connected
 *        device.
 *
 * When the connection is established (see \ref protocol_connect), information
 * about the endpoints in the connected device is retrieved. A device
 * compatible with the communication protocol contains at least one endpoint.
 * If a board reports to have no endpoints, the connection won't be
 * established. This function returns the number of endpoints received when
 * the connection was established. The number of endpoints is not queried from
 * the device again.
 *
 * \param[in] protocol_handle  A handle to an open connection returned.
 *
 * \return If the provided handle is valid, a positive number of endpoints is
 *         returned, otherwise a negative error code is returned. This
 *         function won't return 0.
 */
int32_t protocol_get_num_endpoints(int32_t protocol_handle);

/**
 * \brief This function returns the information about the firmware of the
 *        connected device.
 *
 * This function sends a message to the connected device to query information
 * about the firmware and waits for the answer. The reply message is evaluated
 * and firmware information is written into the provided structure.
 *
 * Note: This function allocates memory to hold the firmware description string.
 *       The caller must take core for freeing that memory when it's no longer
 *       needed.
 *
 * \param[in]  protocol_handle  A handle to an open connection returned.
 * \param[out] information      A pointer to the structure, where the firmware
 *                              information will be stored.
 *
 * \return If the provided handle is valid, 0 will be returned and firmware
 *         information will be written to information. Otherwise a negative
 *         error code is returned and pVersion will not be modified.
 */
int32_t protocol_get_firmware_information(int32_t protocol_handle,
                                          Firmware_Information_t* information);

/**
 * \brief This function triggers a firmware reset of the connected device.
 *
 * This function sends a message to the connected device to trigger a firmware
 * ware reset and waits for the answer.
 *
 * After calling this function, \ref protocol_disconnect should be called
 * immediately to ensure a proper reboot of the connected device.
 *
 * \param[in]  protocol_handle  A handle to an open connection returned.
 *
 * \return If the provided handle is valid and the reset feature is supported
 *         by the connected device, 0 will be returned. Otherwise an error code
 *         is returned.
 */
int32_t protocol_do_firmware_reset(int32_t protocol_handle);

/**
 * \brief This function returns information about the requested endpoint.
 *
 * When the connection is established (see \ref protocol_connect), information
 * about the endpoints in the connected device is retrieved. This function
 * returns information about the requested endpoints that was received when
 * the connection was established. The endpoint information is not queried
 * from the device again.
 *
 * The caller must provide a valid pointer to a structure of type
 * \ref Endpoint_Info_t. That structure may be uninitialized.
 *
 * \param[in]  protocol_handle  A handle to an open connection.
 * \param[in]  endpoint         The number of the requested endpoint. (First
 *                              endpoint is 1, not 0!)
 * \param[out] endpoint_info    A pointer to the structure, where the endpoint
 *                              information will be written to.
 *
 * \return If the provided handle is valid, and the requested endpoint exists,
 *         0 will be returned and endpoint information will be written to
 *         endpoint_info. Otherwise a negative error code is returned and
 *         endpoint_info will not be modified.
 */
int32_t protocol_get_endpoint_info(int32_t protocol_handle, uint8_t endpoint,
                                   Endpoint_Info_t* endpoint_info);

/**
 * \brief This function returns a human readable description of a status or
 *        error code.
 *
 * Whenever a protocol or endpoint function returns a negative error code or a
 * positive status code sent by an endpoint in a connected device, this
 * function can be used to get a human readable description of that code.
 *
 * The protocol handle is needed when retrieving a description for a status
 * code sent by the device. When retrieving the description of a negative
 * error code, the handle is ignored. This allows to retrieve error
 * description in case a connection could not be established.
 *
 * \param[in] protocol_handle  A handle to an open connection.
 * \param[in] status_code      The status or error code to be described.
 *
 * \return This function returns a pointer to a zero-terminated string
 *         containing the human readable description. The caller must not free
 *         or modify the memory pointed to.
 */
const char* protocol_get_status_code_description(int32_t protocol_handle,
                                                 int32_t status_code);

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

/* End of include guard */
#endif /* PROTOCOL_H_INCLUDED */

/* --- End of File -------------------------------------------------------- */
