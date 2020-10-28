/**
 * \file EndpointRadarErrorCodes.h
 *
 * \brief This file defines the status codes of the radar endpoints.
 *
 * The modules EndpointRadarXXX share the same status codes. This files lists
 * the known error codes.
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

#ifndef ENDPOINTRADARSTATUSCODES_H_INCLUDED
#define ENDPOINTRADARSTATUSCODES_H_INCLUDED

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/

/* Enable C linkage if header is included in C++ files */
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * \defgroup EndpointRadarErrorCodes EndpointRadarErrorCodes
 *
 * \brief An endpoint of radar type can return of these status codes.
 *
 * @{
 */

/*
==============================================================================
   2. DEFINITIONS
==============================================================================
*/

/**
 * The requested operation can't be executed. A possible reason is that a
 * certain test mode is activated or the automatic trigger is active.
 */
#define EP_RADAR_ERR_BUSY                        0x0002

/**
 * The requested operation is not supported by the currently active mode of
 *operation.
 */
#define EP_RADAR_ERR_INCOMPATIBLE_MODE           0x0003

/**
 * A timeout has occurred while waiting for a data frame to be acquired.
 */
#define EP_RADAR_ERR_TIME_OUT                    0x0004

/**
 * The requested time interval between two frames is out of range.
 */
#define EP_RADAR_ERR_UNSUPPORTED_FRAME_INTERVAL  0x0005

/**
 * One or more of the selected RX or TX antennas is not present on the device.
 */
#define EP_RADAR_ERR_ANTENNA_DOES_NOT_EXIST      0x0006

/**
 * The requested temperature sensor does not exist.
 */
#define EP_RADAR_ERR_SENSOR_DOES_NOT_EXIST       0x0007

/**
 * The combination of chirps per frame, samples per chirp and number of
 * antennas is not supported by the driver. A possible reason is the limit of
 * the driver internal data memory.
 */
#define EP_RADAR_ERR_UNSUPPORTED_FRAME_FORMAT    0x0008

/**
 * The specified RF frequency is not in the supported range of the device.
 */
#define EP_RADAR_ERR_FREQUENCY_OUT_OF_RANGE      0x0009

/**
 * The specified transmission power is not in the valid range of
 * 0...max_tx_power (see \ref Device_Info_t).
 */
#define EP_RADAR_ERR_POWER_OUT_OF_RANGE          0x000A

/**
 * The device is not capable to capture the requested part of the complex
 *signal (see \ref Device_Info_t).
 */
#define EP_RADAR_ERR_UNAVAILABLE_SIGNAL_PART     0x000B

/**
 * The specified FMCW ramp direction is not supported by the device. */
#define EP_RADAR_ERR_UNSUPPORTED_DIRECTION       0x0020

/**
 * The specified sampling rate is out of range.
 */
#define EP_RADAR_ERR_SAMPLERATE_OUT_OF_RANGE     0x0050

/**
 * The specified sample resolution is out of range.
 */
#define EP_RADAR_ERR_UNSUPPORTED_RESOLUTION      0x0051

/**
 * The specified TX mode is not supported by the device.
 */
#define EP_RADAR_ERR_UNSUPPORTED_TX_MODE         0x0100

/**
 * The specified high pass filter gain is not defined.
 */
#define EP_RADAR_ERR_UNSUPPORTED_HP_GAIN         0x0101

/**
 * The specified high pass filter cut-off frequency is not defined.
 */
#define EP_RADAR_ERR_UNSUPPORTED_HP_CUTOFF       0x0102

/**
 * The specified gain adjustment setting is not defined.
 */
#define EP_RADAR_ERR_UNSUPPORTED_VGA_GAIN        0x0103

/**
 * The specified reset timer period is out of range.
 */
#define EP_RADAR_ERR_RESET_TIMER_OUT_OF_RANGE    0x0104

/**
 * The specified charge pump current is out of range.
 */
#define EP_RADAR_ERR_INVALID_CHARGE_PUMP_CURRENT 0x0105

/**
 * The specified charge pump pulse width is not defined.
 */
#define EP_RADAR_ERR_INVALID_PULSE_WIDTH         0x0106

/**
 * The specified modulator order is not defined.
 */
#define EP_RADAR_ERR_INVALID_FRAC_ORDER          0x0107

/**
 * The specified dither mode is not defined.
 */
#define EP_RADAR_ERR_INVALID_DITHER_MODE         0x0108

/**
 * The specified cycle slip reduction mode is not defined.
 */
#define EP_RADAR_ERR_INVALID_CYCLE_SLIP_MODE     0x0109

/**
 * The calibration of phase settings or base band chain did not succeed.
 */
#define EP_RADAR_ERR_CALIBRATION_FAILED          0x010A

/**
 * The provided oscillator phase setting is not valid. It's forbidden to
 * disable both phase modulators.
 */
#define EP_RADAR_ERR_INVALID_PHASE_SETTING       0x010B

/**
 * The specified ADC tracking mode is not supported by the device.
 */
#define EP_RADAR_ERR_UNDEFINED_TRACKING_MODE     0x0110

/**
 * The specified ADC sampling time is not supported by the device.
 */
#define EP_RADAR_ERR_UNDEFINED_ADC_SAMPLE_TIME   0x0111

/**
 * The specified ADC oversampling factors is not supported by the device.
 */
#define EP_RADAR_ERR_UNDEFINED_ADC_OVERSAMPLING  0x0112

/**
 * The specified shape sequence is not supported. There must not be a gap
 * between used shapes.
 */
#define EP_RADAR_ERR_NONCONTINUOUS_SHAPE_SEQUENCE 0x0120

/**
 * One or more specified number of repetition is not supported. Only powers of
 * two are allowed. Total numbers of shape groups must not exceed 4096.
 */
#define EP_RADAR_ERR_UNSUPPORTED_NUM_REPETITIONS  0x0121

/**
 * One or more of the specified power modes is not supported.
 */
#define EP_RADAR_ERR_UNSUPPORTED_POWER_MODE       0x0122

/**
 * One or more of the specified post shape / post shape set delays is not
 * supported.
 */
#define EP_RADAR_ERR_POST_DELAY_OUT_OF_RANGE      0x0123

/**
 * The specified number of frames is out of range.
 */
#define EP_RADAR_ERR_NUM_FRAMES_OUT_OF_RANGE      0x0124

/**
 * The requested shape does not exist.
 */
#define EP_RADAR_ERR_SHAPE_NUMBER_OUT_OF_RANGE    0x0125

/**
 * The specified pre-chirp delay is out of range.
 */
#define EP_RADAR_ERR_PRECHIRPDELAY_OUT_OF_RANGE   0x0126

/**
 * The specified post-chirp delay is out of range.
 */
#define EP_RADAR_ERR_POSTCHIRPDELAY_OUT_OF_RANGE  0x0127

/**
 * The specified PA delay is out of range.
 */
#define EP_RADAR_ERR_PADELAY_OUT_OF_RANGE         0x0128

/**
 * The specified ADC delay is out of range.
 */
#define EP_RADAR_ERR_ADCDELAY_OUT_OF_RANGE        0x0129

/**
 * The specified wake up time is out of range.
 */
#define EP_RADAR_ERR_WAKEUPTIME_OUT_OF_RANGE      0x012A

/**
 * The specified PLL settle time is out of range.
 */
#define EP_RADAR_ERR_SETTLETIME_OUT_OF_RANGE      0x012B

/**
 * The specified FIFO slice size is not supported.
 */
#define EP_RADAR_ERR_UNSUPPORTED_FIFO_SLICE_SIZE  0x012C

/**
 * The FIFO slice can't be released. It has not been used.
 */
#define EP_RADAR_ERR_SLICES_NOT_RELEASABLE        0x012D

/**
 * A FIFO overflow has occurred. A reset is needed.
 */
#define EP_RADAR_ERR_FIFO_OVERFLOW                0x012E

/**
 * No memory buffer has been provided to store the radar data.
 */
#define EP_RADAR_ERR_NO_MEMORY                    0x012F

/**
 * The received SPI data sequence does not match the expected one.
 */
#define EP_RADAR_ERR_SPI_TEST_FAILED              0x0130

/**
 * The chip could not be programmed.
 */
#define EP_RADAR_ERR_CHIP_SETUP_FAILED            0x0131

/**
 * The On-Chip FIFO memory test faild.
 */
#define EP_RADAR_ERR_MEMORY_TEST_FAILED           0x0132

/**
 * The Device is not supported by the driver.
 */
#define EP_RADAR_ERR_DEVICE_NOT_SUPPORTED         0x0133

/**
 * The requested feature is not supported by the connected device.
 */
#define EP_RADAR_ERR_FEATURE_NOT_SUPPORTED        0x0134

/**
 * The PA Delay is shorter than the pre chirp delay.
 */
#define EP_RADAR_ERR_PRECHIRP_EXCEEDS_PADELAY     0x0135

/**
 * The register selected for override does not exist.
 */
#define EP_RADAR_ERR_REGISTER_OUT_OF_RANGE        0x0136

/** @} */

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

/* End of include guard */
#endif /* ENDPOINTRADARSTATUSCODES_H_INCLUDED */

/* --- End of File -------------------------------------------------------- */
