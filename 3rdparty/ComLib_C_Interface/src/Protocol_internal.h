/**
 * \file Protocol_internal.h
 *
 * \brief This file defines the internal protocol API to be used by endpoint
 *        implementations.
 *
 * This header announces those types and functions implemented in the module
 * \ref Protocol.c that are meant to be used only by endpoint implementations.
 * Don't include it from user code.
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

#ifndef PROTOCOL_INTERNAL_H_INCLUDED
#define PROTOCOL_INTERNAL_H_INCLUDED

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/
#ifndef __PROTOCOL_INCLUDE_ENDPOINT_ONLY_API__
#error This file must not be included directly. Only endpoint implementations may include it.
#endif

#include <stdint.h>

/* Enable C linkage if header is included in C++ files */
#ifdef __cplusplus
extern "C"
{
#else
    /* for Visual Studio, bring inline for C */
    #ifdef _MSC_VER
    #define inline __inline
    #endif

    #define __MACHINE_IS_LITTLE_ENDIAN__
#endif /* __cplusplus */

/*
==============================================================================
   3. TYPES
==============================================================================
*/

/**
 * \internal
 * \brief This structure contains information about an endpoint implementation
 *
 * Each endpoint implementation must contain an instance of this type that
 * holds information about the endpoint implementation needed by the module
 * \ref Protocol.c. That instance must be announced to the protocol module by
 * adding a pointer to it in \ref known_endpoints
 * (see \ref Protocol_KnownEndpoints.h).
 */
 
 ///@cond INTERNAL
 
typedef struct
{
    uint32_t type;                                /**< The unique code of the
                                                       implemented endpoint
                                                       type. */
    uint16_t min_version;                         /**< The first version of
                                                       the implemented
                                                       endpoint type supported
                                                       by the host side
                                                       implementation. */
    uint16_t max_version;                         /**< The last version of the
                                                       implemented endpoint
                                                       type supported by the
                                                       host side
                                                       implementation. */
    const char* description;                      /**< A zero-terminated
                                                       string containing a
                                                       human readable
                                                       description of the
                                                       implemented endpoint
                                                       type. */
    void (*parse_payload)(int32_t protocol_handle,
                          uint8_t endpoint,
                          const uint8_t* payload,
                          uint16_t payload_size); /**< A pointer to the
                                                       function that is called
                                                       any time a payload
                                                       message is received
                                                       from an endpoint of the
                                                       according type. */
    const char* (*get_status_descr)(uint16_t error_code);
                                                  /**< A pointer to a function
                                                       that returns a human
                                                       readable description
                                                       for any status code an
                                                       endpoint of the
                                                       implemented type can
                                                       send. */
} Endpoint_Definition_t;

///@endcond

/*
==============================================================================
   5. FUNCTION PROTOTYPES AND INLINE FUNCTIONS
==============================================================================
*/

/**
 * \internal
 * \brief This function checks if an endpoint in the device is compatible with
 *        a host side implementation
 *
 * The type and version information for the requested endpoint reported by the
 * device is compared with the provided endpoint definition structure. The
 * result of that comparison is returned as an according error code.
 *
 * \param[in] protocol_handle     A handle to an open connection.
 * \param[in] endpoint            The requested endpoint whose compatibility
 *                                is to be checked.
 * \param[in] endpoint_definiton  A pointer to the information about the host
 *                                side endpoint implementation.
 *
 * \return If the endpoint in the device is compatible with the host side
 *         implementation, 0 is returned. If the endpoint is not compatible a
 *         negative error code describing the reason of incompatibility is
 *         returned.
 */
int32_t protocol_is_endpoint_compatible(int32_t protocol_handle,
                                        uint8_t endpoint,
                                        const Endpoint_Definition_t*
                                          endpoint_definiton);

/**
 * \internal
 * \brief This function sends a message to a device and receives the responded
 *        messages.
 *
 * The function sends a payload message to the requested endpoint in the
 * connected device and reads all payload messages sent from the device in
 * response until the device sends a status message.
 *
 * For each received payload message the callback parse_payload is called from
 * that endpoint implementation that matches type and version of the device's
 * endpoint that sent the message. If no endpoint implementation on the host
 * side matches the device's endpoint the message is dropped.
 *
 * Before the message is actually sent to the device, the functions checks if
 * the addressed endpoint is compatible with the host side endpoint
 * implementation provided by the caller.
 *
 * \param[in] protocol_handle     A handle to an open connection.
 * \param[in] endpoint            The endpoint in the device to which the
 *                                message will be sent.
 * \param[in] endpoint_definiton  A pointer to the definition of an host side
 *                                endpoint implementation, that the device's
 *                                endpoint will be checked against for
 *                                compatibility.
 * \param[in] payload             A pointer to a buffer containing the payload
 *                                to be sent to the device.
 * \param[in] payload_size        The number of data bytes in payload buffer.
 *
 * \return If a status message is received from the device the function
 *         returns a positive number containing the 16 bit status code in
 *         bits 0...15 and the endpoint that sent the code in bits 16...23.
 *         If an error occurred a negative error code is returned.
 */
int32_t protocol_send_and_receive(int32_t protocol_handle,
                                  uint8_t endpoint,
                                  const Endpoint_Definition_t*
                                    endpoint_definiton,
                                  const uint8_t* payload,
                                  uint16_t payload_size);

/**
 * \internal
 * \brief The function extracts a signed 8 bit integer from a payload buffer.
 *
 * The function reads one byte from the given payload buffer at the requested
 * position and interprets it as a signed 8 bit integer value, which will be
 * returned.
 *
 * \param[in] payload  A pointer to a block of payload data.
 * \param[in] offset   The byte position at which the requested data value
 *                     starts.
 *
 * \return The function returns the value read from the payload.
 */
static inline int8_t protocol_read_payload_int8(const uint8_t* payload,
                                                uint16_t offset)
{
    return (int8_t)payload[offset];
}

/**
 * \internal
 * \brief The function extracts an unsigned 8 bit integer from a payload
 *        buffer.
 *
 * The function reads one byte from the given payload buffer at the requested
 * position and interprets it as an unsigned 8 bit integer value, which will
 * be returned.
 *
 * \param[in] payload  A pointer to a block of payload data.
 * \param[in] offset   The byte position at which the requested data value
 *                     starts.
 *
 * \return The function returns the value read from the payload.
 */
static inline uint8_t protocol_read_payload_uint8(const uint8_t* payload,
                                                  uint16_t offset)
{
    return payload[offset];
}

/**
 * \internal
 * \brief The function extracts a signed 16 bit integer from a payload buffer.
 *
 * The function reads two bytes from the given payload buffer starting at the
 * requested position and interprets them as a signed 16 bit integer value,
 * which will be returned. If needed the byte order is swapped to convert from
 * the payload's little endian format to the CPU's endian format.
 *
 * \param[in] payload  A pointer to a block of payload data.
 * \param[in] offset   The byte position at which the requested data value
 *                     starts.
 *
 * \return The function returns the value read from the payload.
 */
static inline int16_t protocol_read_payload_int16(const uint8_t* payload,
                                                  uint16_t offset);

/**
 * \internal
 * \brief The function extracts an unsigned 16 bit integer from a payload
 *        buffer.
 *
 * The function reads two bytes from the given payload buffer starting at the
 * requested position and interprets them as an unsigned 16 bit integer value,
 * which will be returned. If needed the byte order is swapped to convert from
 * the payload's little endian format to the CPU's endian format.
 *
 * \param[in] payload  A pointer to a block of payload data.
 * \param[in] offset   The byte position at which the requested data value
 *                     starts.
 *
 * \return The function returns the value read from the payload.
 */
static inline uint16_t protocol_read_payload_uint16(const uint8_t* payload,
                                                    uint16_t offset);

/**
 * \internal
 * \brief The function extracts a signed 32 bit integer from a payload buffer.
 *
 * The function reads four bytes from the given payload buffer starting at the
 * requested position and interprets them as a signed 32 bit integer value,
 * which will be returned. If needed the byte order is swapped to convert from
 * the payload's little endian format to the CPU's endian format.
 *
 * \param[in] payload  A pointer to a block of payload data.
 * \param[in] offset   The byte position at which the requested data value
 *                     starts.
 *
 * \return The function returns the value read from the payload.
 */
static inline int32_t protocol_read_payload_int32(const uint8_t* payload,
                                                  uint16_t offset);

/**
 * \internal
 * \brief The function extracts an unsigned 32 bit integer from a payload
 *        buffer.
 *
 * The function reads four bytes from the given payload buffer starting at the
 * requested position and interprets them as an unsigned 32 bit integer value,
 * which will be returned. If needed the byte order is swapped to convert from
 * the payload's little endian format to the CPU's endian format.
 *
 * \param[in] payload  A pointer to a block of payload data.
 * \param[in] offset   The byte position at which the requested data value
 *                     starts.
 *
 * \return The function returns the value read from the payload.
 */
static inline uint32_t protocol_read_payload_uint32(const uint8_t* payload,
                                                    uint16_t offset);

/**
 * \internal
 * \brief The function extracts a signed 64 bit integer from a payload buffer.
 *
 * The function reads four bytes from the given payload buffer starting at the
 * requested position and interprets them as a signed 64 bit integer value,
 * which will be returned. If needed the byte order is swapped to convert from
 * the payload's little endian format to the CPU's endian format.
 *
 * \param[in] payload  A pointer to a block of payload data.
 * \param[in] offset   The byte position at which the requested data value
 *                     starts.
 *
 * \return The function returns the value read from the payload.
 */
static inline int64_t protocol_read_payload_int64(const uint8_t* payload,
                                                  uint16_t offset);

/**
 * \internal
 * \brief The function extracts an unsigned 64 bit integer from a payload
 *        buffer.
 *
 * The function reads four bytes from the given payload buffer starting at the
 * requested position and interprets them as an unsigned 64 bit integer value,
 * which will be returned. If needed the byte order is swapped to convert from
 * the payload's little endian format to the CPU's endian format.
 *
 * \param[in] payload  A pointer to a block of payload data.
 * \param[in] offset   The byte position at which the requested data value
 *                     starts.
 *
 * \return The function returns the value read from the payload.
 */
static inline uint64_t protocol_read_payload_uint64(const uint8_t* payload,
                                                    uint16_t offset);

/**
 * \internal
 * \brief The function writes a signed 8 bit integer value to a payload
 *        buffer.
 *
 * The function writes the given value into the given payload buffer at the
 * requested position.
 *
 * \param[in] payload  A pointer to a block of payload data.
 * \param[in] offset   The byte position at which the value will be written
 *                     to.
 * \param[in] value    The value to be written into the payload buffer.
 */
static inline void protocol_write_payload_int8(uint8_t* payload,
                                               uint16_t offset,
                                               int8_t value)
{
    payload[offset] = (uint8_t)value;
}

/**
 * \internal
 * \brief The function writes an unsigned 8 bit integer value to a payload
 *        buffer.
 *
 * The function writes the given value into the given payload buffer at the
 * requested position.
 *
 * \param[in] payload  A pointer to a block of payload data.
 * \param[in] offset   The byte position at which the value will be written
 *                     to.
 * \param[in] value    The value to be written into the payload buffer.
 */
static inline void protocol_write_payload_uint8(uint8_t* payload,
                                                uint16_t offset,
                                                uint8_t value)
{
    payload[offset] = value;
}

/**
 * \internal
 * \brief The function writes a signed 16 bit integer value to a payload
 *        buffer.
 *
 * The function writes the given value into the given payload buffer at the
 * requested position. If needed the byte order is swapped to convert from the
 * payload's little endian format to the CPU's endian format.
 *
 * \param[in] payload  A pointer to a block of payload data.
 * \param[in] offset   The byte position at which the value will be written
 *                     to.
 * \param[in] value    The value to be written into the payload buffer.
 */
static inline void protocol_write_payload_int16(uint8_t* payload,
                                                uint16_t offset,
                                                int16_t value);

/**
 * \internal
 * \brief The function writes an unsigned 16 bit integer value to a payload
 *        buffer.
 *
 * The function writes the given value into the given payload buffer at the
 * requested position. If needed the byte order is swapped to convert from the
 * payload's little endian format to the CPU's endian format.
 *
 * \param[in] payload  A pointer to a block of payload data.
 * \param[in] offset   The byte position at which the value will be written
 *                     to.
 * \param[in] value    The value to be written into the payload buffer.
 */
static inline void protocol_write_payload_uint16(uint8_t* payload,
                                                 uint16_t offset,
                                                 uint16_t value);

/**
 * \internal
 * \brief The function writes a signed 32 bit integer value to a payload
 *        buffer.
 *
 * The function writes the given value into the given payload buffer at the
 * requested position. If needed the byte order is swapped to convert from the
 * payload's little endian format to the CPU's endian format.
 *
 * \param[in] payload  A pointer to a block of payload data.
 * \param[in] offset   The byte position at which the value will be written
 *                     to.
 * \param[in] value    The value to be written into the payload buffer.
 */
static inline void protocol_write_payload_int32(uint8_t* payload,
                                                uint16_t offset,
                                                int32_t value);

/**
 * \internal
 * \brief The function writes an unsigned 32 bit integer value to a payload
 *        buffer.
 *
 * The function writes the given value into the given payload buffer at the
 * requested position. If needed the byte order is swapped to convert from the
 * payload's little endian format to the CPU's endian format.
 *
 * \param[in] payload  A pointer to a block of payload data.
 * \param[in] offset   The byte position at which the value will be written
 *                     to.
 * \param[in] value    The value to be written into the payload buffer.
 */
static inline void protocol_write_payload_uint32(uint8_t* payload,
                                                 uint16_t offset,
                                                 uint32_t value);

/**
 * \internal
 * \brief The function writes a signed 64 bit integer value to a payload
 *        buffer.
 *
 * The function writes the given value into the given payload buffer at the
 * requested position. If needed the byte order is swapped to convert from the
 * payload's little endian format to the CPU's endian format.
 *
 * \param[in] payload  A pointer to a block of payload data.
 * \param[in] offset   The byte position at which the value will be written
 *                     to.
 * \param[in] value    The value to be written into the payload buffer.
 */
static inline void protocol_write_payload_int64(uint8_t* payload,
                                                uint16_t offset,
                                                int64_t value);

/**
 * \internal
 * \brief The function writes an unsigned 64 bit integer value to a payload
 *        buffer.
 *
 * The function writes the given value into the given payload buffer at the
 * requested position. If needed the byte order is swapped to convert from the
 * payload's little endian format to the CPU's endian format.
 *
 * \param[in] payload  A pointer to a block of payload data.
 * \param[in] offset   The byte position at which the value will be written
 *                     to.
 * \param[in] value    The value to be written into the payload buffer.
 */
static inline void protocol_write_payload_uint64(uint8_t* payload,
                                                 uint16_t offset,
                                                 uint64_t value);

#ifdef __MACHINE_IS_LITTLE_ENDIAN__
/*
 * The following function implementations wan only be used on machines with
 * little endian CPUs, but are faster than the generic implementations.
 */

static inline int16_t protocol_read_payload_int16(const uint8_t* payload,
                                                  uint16_t offset)
{
    return *(int16_t*)(payload + offset);
}

static inline uint16_t protocol_read_payload_uint16(const uint8_t* payload,
                                                    uint16_t offset)
{
    return *(uint16_t*)(payload + offset);
}

static inline int32_t protocol_read_payload_int32(const uint8_t* payload,
                                                  uint16_t offset)
{
    return *(int32_t*)(payload + offset);
}

static inline uint32_t protocol_read_payload_uint32(const uint8_t* payload,
                                                    uint16_t offset)
{
    return *(uint32_t*)(payload + offset);
}

static inline int64_t protocol_read_payload_int64(const uint8_t* payload,
                                                  uint16_t offset)
{
    return *(int64_t*)(payload + offset);
}

static inline uint64_t protocol_read_payload_uint64(const uint8_t* payload,
                                                    uint16_t offset)
{
    return *(uint64_t*)(payload + offset);
}

static inline void protocol_write_payload_int16(uint8_t* payload,
                                                uint16_t offset,
                                                int16_t iValue)
{
    *(int16_t*)(payload + offset) = iValue;
}

static inline void protocol_write_payload_uint16(uint8_t* payload,
                                                 uint16_t offset,
                                                 uint16_t value)
{
    *(uint16_t*)(payload + offset) = value;
}

static inline void protocol_write_payload_int32(uint8_t* payload,
                                                uint16_t offset,
                                                int32_t value)
{
    *(int32_t*)(payload + offset) = value;
}

static inline void protocol_write_payload_uint32(uint8_t* payload,
                                                 uint16_t offset,
                                                 uint32_t value)
{
    *(uint32_t*)(payload + offset) = value;
}

static inline void protocol_write_payload_int64(uint8_t* payload,
                                                uint16_t offset,
                                                int64_t value)
{
    *(int64_t*)(payload + offset) = value;
}

static inline void protocol_write_payload_uint64(uint8_t* payload,
                                                 uint16_t offset,
                                                 uint64_t value)
{
    *(uint64_t*)(payload + offset) = value;
}

#else
/*
 * The following function implementations are safe on any platform, but slower
 * than the native little endian implementations.
 */

static inline int16_t protocol_read_payload_int16(const uint8_t* payload,
                                                  uint16_t offset)
{
    return (int16_t)((((uint16_t)payload[offset + 1]) <<  8) |
                      ((uint16_t)payload[offset    ]));
}

static inline uint16_t protocol_read_payload_uint16(const uint8_t* payload,
                                                    uint16_t offset)
{
    return (((uint16_t)payload[offset + 1]) <<  8) |
            ((uint16_t)payload[offset    ]);
}

static inline int32_t protocol_read_payload_int32(const uint8_t* payload,
                                                  uint16_t offset)
{
    return (int32_t)((((uint32_t)payload[offset + 3]) << 24) |
                     (((uint32_t)payload[offset + 2]) << 16) |
                     (((uint32_t)payload[offset + 1]) <<  8) |
                      ((uint32_t)payload[offset    ]));
}

static inline uint32_t protocol_read_payload_uint32(const uint8_t* payload,
                                                    uint16_t offset)
{
    return (((uint32_t)payload[offset + 3]) << 24) |
           (((uint32_t)payload[offset + 2]) << 16) |
           (((uint32_t)payload[offset + 1]) <<  8) |
            ((uint32_t)payload[offset    ]);
}

static inline int64_t protocol_read_payload_int64(const uint8_t* payload,
                                                  uint16_t offset)
{
    return (int64_t)((((uint64_t)payload[offset + 7]) << 56) |
                     (((uint64_t)payload[offset + 6]) << 48) |
                     (((uint64_t)payload[offset + 5]) << 40) |
                     (((uint64_t)payload[offset + 4]) << 32) |
                     (((uint64_t)payload[offset + 3]) << 24) |
                     (((uint64_t)payload[offset + 2]) << 16) |
                     (((uint64_t)payload[offset + 1]) <<  8) |
                      ((uint64_t)payload[offset    ]));
}

static inline uint32_t protocol_read_payload_uint32(const uint8_t* payload,
                                                    uint16_t offset)
{
    return (((uint64_t)payload[offset + 7]) << 56) |
           (((uint64_t)payload[offset + 6]) << 48) |
           (((uint64_t)payload[offset + 5]) << 40) |
           (((uint64_t)payload[offset + 4]) << 32) |
           (((uint64_t)payload[offset + 3]) << 24) |
           (((uint64_t)payload[offset + 2]) << 16) |
           (((uint64_t)payload[offset + 1]) <<  8) |
            ((uint64_t)payload[offset    ]);
}

static inline void protocol_write_payload_int16(uint8_t* payload,
                                                uint16_t offset,
                                                int16_t value)
{
    payload[offset + 1] = (((uint16_t)value) >>  8) & 0xFF;
    payload[offset    ] =  ((uint16_t)value)        & 0xFF;
}

static inline void protocol_write_payload_uint16(uint8_t* payload,
                                                 uint16_t offset,
                                                 uint16_t value)
{
    payload[offset + 1] = (value >>  8) & 0xFF;
    payload[offset    ] =  value        & 0xFF;
}

static inline void protocol_write_payload_int32(uint8_t* payload,
                                                uint16_t offset,
                                                int32_t value)
{
    payload[offset + 3] = (((uint32_t)value) >> 24) & 0xFF;
    payload[offset + 2] = (((uint32_t)value) >> 16) & 0xFF;
    payload[offset + 1] = (((uint32_t)value) >>  8) & 0xFF;
    payload[offset    ] =  ((uint32_t)value)        & 0xFF;
}

static inline void protocol_write_payload_uint32(uint8_t* payload,
                                                 uint16_t offset,
                                                 uint32_t value)
{
    payload[offset + 3] = (value >> 24) & 0xFF;
    payload[offset + 2] = (value >> 16) & 0xFF;
    payload[offset + 1] = (value >>  8) & 0xFF;
    payload[offset    ] =  value        & 0xFF;
}

static inline void protocol_write_payload_int64(uint8_t* payload,
                                                uint16_t offset,
                                                int64_t value)
{
    payload[offset + 7] = (((uint64_t)value) >> 56) & 0xFF;
    payload[offset + 6] = (((uint64_t)value) >> 48) & 0xFF;
    payload[offset + 5] = (((uint64_t)value) >> 40) & 0xFF;
    payload[offset + 4] = (((uint64_t)value) >> 32) & 0xFF;
    payload[offset + 3] = (((uint64_t)value) >> 24) & 0xFF;
    payload[offset + 2] = (((uint64_t)value) >> 16) & 0xFF;
    payload[offset + 1] = (((uint64_t)value) >>  8) & 0xFF;
    payload[offset    ] =  ((uint64_t)value)        & 0xFF;
}

static inline void protocol_write_payload_uint64(uint8_t* payload,
                                                 uint16_t offset,
                                                 uint64_t value)
{
    payload[offset + 7] = (value >> 56) & 0xFF;
    payload[offset + 6] = (value >> 48) & 0xFF;
    payload[offset + 5] = (value >> 40) & 0xFF;
    payload[offset + 4] = (value >> 32) & 0xFF;
    payload[offset + 3] = (value >> 24) & 0xFF;
    payload[offset + 2] = (value >> 16) & 0xFF;
    payload[offset + 1] = (value >>  8) & 0xFF;
    payload[offset    ] =  value        & 0xFF;
}

#endif

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

/* End of include guard */
#endif /* PROTOCOL_INTERNAL_H_INCLUDED */

/* --- End of File -------------------------------------------------------- */
