/**
 * \file COMPort.h
 *
 * \brief This file defines the API to access a serial communication port.
 *
 * This interface is an abstraction of a serial communication port. The
 * interface provides basic operations open, close, read and write.
 * Additionally it allows to get a list of available ports.
 *
 * Even though the name is 'COMPort', the actual communication may use a
 * different type of connection (COM, USB, SPI, etc.). The implementation is
 * platform specific and will look differently for each combination of
 * platform and connection type.
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

#ifndef COMPORT_H_INCLUDED
#define COMPORT_H_INCLUDED

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/
#include <stddef.h>
#include <stdint.h>

/* Enable C linkage if header is included in C++ files */
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*
==============================================================================
   5. FUNCTION PROTOTYPES AND INLINE FUNCTIONS
==============================================================================
*/
/**
 * \brief This function returns a list of available COM ports.
 *
 * The port list is compiled into a zero terminated string, where all
 * available ports are separated by a semicolon (';'). The string buffer to
 * hold the port list must be provided by the caller. If the buffer is not
 * sufficient to hold the names of all available ports, it is filled with as
 * much complete port names as possible. Port names will not be truncated.
 *
 * The function will return the number of available COM ports regardless of
 * the number of port names that have been written to the buffer. The caller
 * can count the number of names in the string buffer and compare with the
 * returned number to check if it is sensible to call this function again with
 * a bigger string buffer.
 *
 * \param[out] port_list    A pointer to the string buffer where the port
 *                          names will be written to.
 * \param[in]  buffer_size  The size of the string buffer in bytes.
 *
 * \return The function returns the number of available COM ports. 
 */
uint32_t com_get_port_list(char* port_list, size_t buffer_size);

/**
 * \brief This functions opens the specified COM port.
 *
 * The name of the COM port must be provided as zero terminated string. It is
 * recommended to call \ref com_get_port_list to find the available COM ports
 * and pass one of the returned port names to the open function.
 *
 * \param[in] port_name  A zero terminated string containing the name of the
 *                       COM port
 *
 * \return If the function succeeds, a handle to the opened COM port is
 *         returned. That handle is always zero or positive. If the COM port
 *         can't be opened, a negative value is returned, which is the
 *         forwarded error code from the operating system.
 */
int32_t com_open(const char* port_name);

/**
 * \brief This function closes the specified COM port.
 *
 * The function expects a handle to an open COM port that have been returned
 * by \ref com_open.
 *
 * \param[in] port_handle  The handle to the open COM port that should be
 *                         closed.
 */
void com_close(int32_t port_handle);

/**
 * \brief This function sends data through an open COM port.
 *
 * The function transmits each byte from the provided buffer through the
 * specified COM port and returns after the last byte has been sent.
 *
 * The function expects a handle to an open COM port that have been returned
 * by \ref com_open.
 *
 * \param[in] port_handle  The handle to the open COM port.
 * \param[in] data         A pointer to the buffer containing the data to be
 *                         sent.
 * \param[in] num_bytes    The number of bytes in pBuffer.
 */
void com_send_data(int32_t port_handle, const void* data, size_t num_bytes);

/**
 * \brief This function reads data from an open COM port.
 *
 * The function tries to read the specified number of bytes from an open COM
 * port and stores them in the provided buffer. The function waits for the
 * data, but returns after an internally defined timeout period and returns
 * the number of received bytes.
 *
 * The function expects a handle to an open COM port that have been returned
 * by \ref com_open.
 *
 * \param[in]  port_handle          The handle to the open COM port.
 * \param[out] data                 A pointer to the buffer where the received
 *                                  data will be stored.
 * \param[in]  num_requested_bytes  The number of bytes that should be read
 *                                  from the COM port.
 *
 * \return The function returns the number of bytes that actually have been
 *         received. This number may be smaller than uNumRequestedBytes.
 */
size_t com_get_data(int32_t port_handle,
                    void* data, size_t num_requested_bytes);

/**
 * \brief This function changes the timeout period of an open COM port
 *
 * This function changes the timeout period that will be used when data is read
 * through \ref com_get_data. That function will wait until timeout period is
 * elapsed before returning, if not enough data is received.
 *
 * The function expects a handle to an open COM port that have been returned
 * by \ref com_open.
 *
 * \param[in]  port_handle          The handle to the open COM port.
 * \param[in]  timeout_period_ms    The new timeout period in milli seconds.
 */
void com_set_timeout(int32_t port_handle, uint32_t timeout_period_ms);

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

/* End of include guard */
#endif /* COMPORT_H_INCLUDED */

/* --- End of File -------------------------------------------------------- */
