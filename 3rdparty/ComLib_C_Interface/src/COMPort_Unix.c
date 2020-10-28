/**
 * \file COMPort_Unix.c
 *
 * \brief This file implements the API to access a serial communication port
 *        for macOS and Linux.
 *
 * \see COMPort.h for details
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
#include "COMPort.h"

#if (defined __APPLE__) || (defined LINUX) || (defined __linux__)
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

/*
==============================================================================
   4. DATA
==============================================================================
*/
static int* handles = NULL;               /**< An array of handles to open COM
                                               ports. */
static size_t num_allocated_handles = 0;  /**< The current size of the handle
                                               array. */
static size_t num_open_handles = 0;       /**< The current number of open
                                               handles. */

/*
==============================================================================
   7. EXPORTED FUNCTIONS
==============================================================================
*/

uint32_t com_get_port_list(char* port_list, size_t buffer_size)
{
#if defined __APPLE__
    /*
    * On Mac devices are represented as files in /dev, search for the Com
    * ports (cu) that have a USB in its name
    * Note: use cu* instead of tty* because tty blocks on open for carrier
    *       detection
    */
    const char dev_dir[] = "/dev";
    const char search_pattern[] = "cu.usb";

#else
    /*
    * Modern Linux kernel list serial devices in /dev/serial. This is a nice
    * feature because it can be search for IDs containing Infineon ID.
    */
    const char dev_dir[] = "/dev/serial/by-id";
    const char search_pattern[] = "IFX_CDC";

#endif

    uint32_t num_available_ports = 0;
    DIR* dir_handle;

    /* init port list to empty string */
    if (buffer_size > 0)
    {
        port_list[0] = 0;
    }

    /* open directory that contains the serial device files */
    dir_handle = opendir(dev_dir);

    if (dir_handle != NULL)
    {
        /* read entry per entry */
        struct dirent* dir_entry;

        while ((dir_entry = readdir(dir_handle)) != NULL)
        {
            size_t name_length;

            /* compare device file name with the search pattern */
            char current_device[128];

            if (strstr(dir_entry->d_name, search_pattern) == NULL)
            {
                continue;
            }

            /* add the port name to the list, if buffer capacity is
             * sufficient
             */
            strcpy(current_device, dev_dir);
            strcat(current_device, "/");
            strcat(current_device, dir_entry->d_name);

            name_length = strlen(current_device);

            if (num_available_ports > 0)
            {
                /* add one for the separator */
                ++name_length;
            }

            if (name_length < buffer_size)
            {
                if (num_available_ports > 0)
                {
                    strcat(port_list, ";");
                }

                strcat(port_list, current_device);

                /* update remaining capacity of string buffer */
                buffer_size -= name_length;
            }

            /* port is available, so increase counter */
            ++num_available_ports;
        }

        /* close directory stream */
        closedir(dir_handle);
    }

    return num_available_ports;
}

int32_t com_open(const char* port_name)
{
    int com_port_handle;
    struct termios options;
    int32_t new_handle;

    /*
     * Open the serial port read/write, with no controlling terminal, and
     * don't wait for a connection. See open(2) ("man 2 open") for details.
     */
    com_port_handle = open(port_name, O_RDWR | O_NOCTTY);

    if (com_port_handle == -1)
    {
        return -1;
    }

    /*
     * Note that open() follows POSIX semantics: multiple open() calls to the
     * same file will succeed unless the TIOCEXCL ioctl is issued. This will
     * prevent additional opens except by root-owned processes.
     * See tty(4) ("man 4 tty") and ioctl(2) ("man 2 ioctl") for details.
     */
    if (ioctl(com_port_handle, TIOCEXCL) == -1)
    {
        close(com_port_handle);
        return -1;
    }

    /* Get the current options and save them so we can restore the default
     * settings later.
     */
    if (tcgetattr(com_port_handle, &options) == -1)
    {
        close(com_port_handle);
        return -1;
    }

    /*
     * The serial port attributes such as timeouts and baud rate are set by
     * modifying the termios structure and then calling tcsetattr to  cause
     * the changes to take effect. Note that the changes will not take effect
     * without the tcsetattr() call. See tcsetattr(4) ("man 4 tcsetattr") for
     * details.
     *
     * Set raw input (non-canonical) mode, with reads blocking until either a
     * single character has been received or a one second timeout expires.
     * See tcsetattr(4) ("man 4 tcsetattr") and termios(4) ("man 4 termios")
     * for details.
     */
    cfmakeraw(&options);
    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = 10;

    /* Enable local mode, because the USBD_VCOM APP does not handle the
     * virtual flow control lines
     */
    options.c_cflag |= CLOCAL;

    /* Cause the new options to take effect immediately. */
    if (tcsetattr(com_port_handle, TCSANOW, &options) == -1)
    {
        close(com_port_handle);
        return -1;
    }

    /* add the handle to the array of open COM ports */
    /* --------------------------------------------- */

    /* increase capacity of handle array if needed */
    if (num_open_handles == num_allocated_handles)
    {
        int* new_handles = (int*)malloc((num_open_handles + 1) * sizeof(int));

        if (num_allocated_handles > 0)
        {
            memcpy(new_handles, handles, sizeof(int) * num_allocated_handles);
        }

        new_handles[num_allocated_handles++] = -1;

        free(handles);
        handles = new_handles;
    }

    /* add new handle to table */
    new_handle = 0;

    while (handles[new_handle] != -1)
    {
        ++new_handle;
    }

    handles[new_handle] = com_port_handle;
    ++num_open_handles;

    return new_handle;
}

void com_close(int32_t port_handle)
{
    /* check if handle is valid */
    if ((port_handle >= 0) && (port_handle < num_allocated_handles) &&
            (handles[port_handle] != -1))
    {
        /* close COM port */
        close(handles[port_handle]);

        /* remove handle from the table */
        handles[port_handle] = -1;
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

void com_send_data(int32_t port_handle, const void* data, size_t num_bytes)
{
    /* check if handle is valid */
    if ((port_handle >= 0) && (port_handle < num_allocated_handles) &&
        (handles[port_handle] != -1))
    {
        /* send data */
        write(handles[port_handle], data, num_bytes);
    }
}

size_t com_get_data(int32_t port_handle,
                    void* data, size_t num_requested_bytes)
{
    /* check if handle is valid */
    if ((port_handle >= 0) && (port_handle < num_allocated_handles) &&
        (handles[port_handle] != -1))
    {
        size_t num_received_bytes = 0;
        char* read_buffer = (char*)data;

        /* read data */
        while (num_received_bytes < num_requested_bytes)
        {
            ssize_t num_bytes = read(handles[port_handle],
                                     read_buffer + num_received_bytes,
                                     num_requested_bytes -
                                       num_received_bytes);

            if (num_bytes <= 0)
            {
                break;
            }

            num_received_bytes += num_bytes;
        }

        return num_received_bytes;
    }

    /* if handle was not valid, return 0 to indicate that no data was
     * received
     */
    return 0;
}

void com_set_timeout(int32_t port_handle, uint32_t timeout_period_ms)
{
    /* check if handle is valid */
    if ((port_handle >= 0) &&
            (port_handle < (int32_t)num_allocated_handles) &&
            (handles[port_handle] != -1))
    {
        /* get current settings */
        struct termios options;
        tcgetattr(handles[port_handle], &options);

        /* apply new timeout value */
        options.c_cc[VTIME] = timeout_period_ms / 100;
        tcsetattr(handles[port_handle], TCSANOW, &options);
    }
}

/* --- Close open blocks -------------------------------------------------- */

/* End of Windows only code */
#endif /* (defined __APPLE__) || (defined LINUX) || (defined __linux__) */

/* --- End of File -------------------------------------------------------- */
