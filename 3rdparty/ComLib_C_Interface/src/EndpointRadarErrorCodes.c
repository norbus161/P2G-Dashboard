/**
 * \file EndpointRadarErrorCodes.c
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
#include "EndpointRadarErrorCodes.h"
#include <stdint.h>

/*
==============================================================================
   7. EXPORTED FUNCTIONS
==============================================================================
*/

/**
 * \internal
 * \brief This function returns a human readable description of a status code.
 *
 * This function is called from \ref protocol_get_status_code_description to
 * retrieve a human readable description for a status code sent by an endpoint
 * that is of type and version compatible to the implementation in this
 * module.
 *
 * \param[in] error_code  The status code to be described.
 *
 * \return This function returns a pointer to a zero-terminated string
 *         containing the human readable description. The caller must not free
 *         or modify the memory pointed to.
 */
const char* ep_radar_get_error_code_description(uint16_t error_code)
{
    switch (error_code)
    {
        case EP_RADAR_ERR_BUSY:
            return "The device is busy. Maybe test mode or automatic trigger is active.";

        case EP_RADAR_ERR_INCOMPATIBLE_MODE:
            return "The requested operation is not supported by the currently active mode of operation.";

        case EP_RADAR_ERR_TIME_OUT:
            return "A timeout has occurred while waiting for a data frame to be acquired.";

        case EP_RADAR_ERR_UNSUPPORTED_FRAME_INTERVAL:
            return "The requested time interval is out of range.";

        case EP_RADAR_ERR_ANTENNA_DOES_NOT_EXIST:
            return "One or more of the selected antennas is not present on the device.";

        case EP_RADAR_ERR_SENSOR_DOES_NOT_EXIST:
            return "The requested temperature sensor does not exist.";

        case EP_RADAR_ERR_UNSUPPORTED_FRAME_FORMAT:
            return "The specified frame format is not supported.";

        case EP_RADAR_ERR_FREQUENCY_OUT_OF_RANGE:
            return "The specified RF frequency is out of range.";

        case EP_RADAR_ERR_POWER_OUT_OF_RANGE:
            return "The specified TX power is out of range.";

        case EP_RADAR_ERR_UNAVAILABLE_SIGNAL_PART:
            return "The device is not capable to capture the requested part of the complex signal.";

        case EP_RADAR_ERR_UNSUPPORTED_DIRECTION:
            return "The specified FMCW ramp direction is not supported by the device.";

        case EP_RADAR_ERR_SAMPLERATE_OUT_OF_RANGE:
            return "The specified sampling rate is out of range.";

        case EP_RADAR_ERR_UNSUPPORTED_RESOLUTION:
            return "The specified ADC resolution is out of range.";

        case EP_RADAR_ERR_UNSUPPORTED_TX_MODE:
            return "The specified TX mode is not supported by the device.";

        case EP_RADAR_ERR_UNSUPPORTED_HP_GAIN:
            return "The specified high pass filter gain is not supported by the device.";

        case EP_RADAR_ERR_UNSUPPORTED_HP_CUTOFF:
            return "The specified high pass filter cutoff frequency is not supported by the device.";

        case EP_RADAR_ERR_UNSUPPORTED_VGA_GAIN:
            return "The specified gain adjustment setting is not supported by the device.";

        case EP_RADAR_ERR_RESET_TIMER_OUT_OF_RANGE:
            return "The specified reset timer period is out of range.";

        case EP_RADAR_ERR_INVALID_CHARGE_PUMP_CURRENT:
            return "The specified charge pump current is out of range.";

        case EP_RADAR_ERR_INVALID_PULSE_WIDTH:
            return "The specified charge pump pulse width is not defined.";

        case EP_RADAR_ERR_INVALID_FRAC_ORDER:
            return "The specified modulator order is not defined.";

        case EP_RADAR_ERR_INVALID_DITHER_MODE:
            return "The specified dither mode is not defined.";

        case EP_RADAR_ERR_INVALID_CYCLE_SLIP_MODE:
            return "The specified cycle slip reduction mode is not defined.";

        case EP_RADAR_ERR_CALIBRATION_FAILED:
            return "The calibration of phase settings or base band chain did not succeed.";

        case EP_RADAR_ERR_INVALID_PHASE_SETTING:
            return "The provided oscillator phase setting is not valid. It's forbidden to disable both phase modulators.";

        case EP_RADAR_ERR_UNDEFINED_TRACKING_MODE:
            return "The specified ADC tracking mode is not supported by the device.";

        case EP_RADAR_ERR_UNDEFINED_ADC_SAMPLE_TIME:
            return "The specified ADC sampling time is not supported by the device.";

        case EP_RADAR_ERR_UNDEFINED_ADC_OVERSAMPLING:
            return "The specified ADC oversampling factors is not supported by the device.";

        case EP_RADAR_ERR_NONCONTINUOUS_SHAPE_SEQUENCE:
            return "The specified shape sequence is not supported. There must not be a gap between used shapes.";

        case EP_RADAR_ERR_UNSUPPORTED_NUM_REPETITIONS:
            return "One or more specified number of repetition is not supported. Only powers of two are allowed. Total numbers of shape groups must not exceed 4096.";

        case EP_RADAR_ERR_UNSUPPORTED_POWER_MODE:
            return "One or more of the specified power modes is not supported.";

        case EP_RADAR_ERR_POST_DELAY_OUT_OF_RANGE:
            return "One or more of the specified post shape / post shape set delays is not supported.";

        case EP_RADAR_ERR_NUM_FRAMES_OUT_OF_RANGE:
            return "The specified number of frames is out of range.";

        case EP_RADAR_ERR_SHAPE_NUMBER_OUT_OF_RANGE:
            return "The requested shape does not exist.";

        case EP_RADAR_ERR_PRECHIRPDELAY_OUT_OF_RANGE:
            return "The specified pre-chirp delay is out of range.";

        case EP_RADAR_ERR_POSTCHIRPDELAY_OUT_OF_RANGE:
            return "The specified post-chirp delay is out of range.";

        case EP_RADAR_ERR_PADELAY_OUT_OF_RANGE:
            return "The specified PA delay is out of range.";

        case EP_RADAR_ERR_ADCDELAY_OUT_OF_RANGE:
            return "The specified ADC delay is out of range.";

        case EP_RADAR_ERR_WAKEUPTIME_OUT_OF_RANGE:
            return "The specified wake up time is out of range.";

        case EP_RADAR_ERR_SETTLETIME_OUT_OF_RANGE:
            return "The specified PLL settle time is out of range.";

        case EP_RADAR_ERR_UNSUPPORTED_FIFO_SLICE_SIZE:
            return "The specified FIFO slice size is not supported.";

        case EP_RADAR_ERR_SLICES_NOT_RELEASABLE:
            return "The FIFO slice can't be released. It has not been used.";

        case EP_RADAR_ERR_FIFO_OVERFLOW:
            return "A FIFO overflow has occurred. A reset is needed.";

        case EP_RADAR_ERR_NO_MEMORY:
            return "No memory buffer has been provided to store the radar data.";

        case EP_RADAR_ERR_SPI_TEST_FAILED:
            return "The received SPI data sequence does not match the expected one.";

        case EP_RADAR_ERR_CHIP_SETUP_FAILED:
            return "The chip could not be programmed.";

        case EP_RADAR_ERR_MEMORY_TEST_FAILED:
            return "The On-Chip FIFO memory test faild.";

        case EP_RADAR_ERR_DEVICE_NOT_SUPPORTED:
            return "The Device is not supported by the driver.";

        case EP_RADAR_ERR_FEATURE_NOT_SUPPORTED:
            return "The requested feature is not supported by the connected device.";

        case EP_RADAR_ERR_PRECHIRP_EXCEEDS_PADELAY:
            return "The PA Delay is shorter than the pre chirp delay.";

        case EP_RADAR_ERR_REGISTER_OUT_OF_RANGE:
            return "The register selected for override does not exist.";

        default:
            return "Unknown Error";
    }
}

/* --- End of File -------------------------------------------------------- */
