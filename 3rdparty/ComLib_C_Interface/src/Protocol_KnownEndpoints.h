/**
 * \internal
 * \file Protocol_KnownEndpoints.h
 *
 * \brief This file links the information about the known endpoints to the
 *        protocol implementation.
 *
 * This file defines a list of all available host side endpoint
 * implementations.
 *
 * This header must not be included by any module except \ref Protocol.c
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
#ifndef __PROTOCOL_INCLUDES_ENDPOINT_LIST__
#error This file must not be included directly. Only Protocol.c may include it.
#endif

/*
 * Those are the info structures about the known endpoints that are defined
 * along with the according endpoint implementations.
 */
extern const Endpoint_Definition_t ep_radar_base_definition;
extern const Endpoint_Definition_t ep_radar_fmcw_definition;
extern const Endpoint_Definition_t ep_radar_doppler_definition;
extern const Endpoint_Definition_t ep_radar_adcxmc_definition;
extern const Endpoint_Definition_t ep_target_detection_definition;
extern const Endpoint_Definition_t ep_calibration_definition;
extern const Endpoint_Definition_t ep_radar_p2g_definition;
extern const Endpoint_Definition_t ep_radar_industrial_definition;

/*
==============================================================================
   4. DATA
==============================================================================
*/

/**
 * \internal
 * \brief This is the list of available endpoint implementations.
 *
 * For each host side endpoint implementation a structure of type
 * \ref Endpoint_Definition_t must be provided. This array compiles them into
 * a list, that will be used by the protocol module do dispatch endpoint
 * specific tasks, such as payload parsing or status code description.
 */
static const Endpoint_Definition_t* known_endpoints[] =
{
    &ep_radar_base_definition,
    &ep_radar_fmcw_definition,
    &ep_radar_doppler_definition,
    &ep_radar_adcxmc_definition,
    &ep_target_detection_definition,
    &ep_calibration_definition,
	&ep_radar_p2g_definition,
	&ep_radar_industrial_definition
};


/* --- End of File -------------------------------------------------------- */
