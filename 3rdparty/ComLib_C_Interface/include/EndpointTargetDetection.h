/**
 * \file EndPointTargetDetection.h
 *
 * \brief This file defines the API to communicate with Radar Target Detection Endpoints
 *        in Infineon 24 GHz radar devices.
 *
 * The module EndPointTargetDetection.c provides functions to communicate with
 * the target detection endpoint in Infineon 24 GHz radar devices. The user must 
 * use the function provided by the module \ref Protocol.c to open a connection 
 * to a radar sensor device and query information about the endpoints in the device.
 *
 * The function \ref ep_targetdetect_is_compatible_endpoint can be used to check
 * each endpoint if it is compatible with this API. If it is the types and
 * functions in from this module can be used to send parameters to the
 * endpoint and query the current DSP settings and radar target information.
 *
 * All data received from the connected device is returned through callbacks.
 * For each message type that can be received from the device, a separate
 * callback function type is defined. The user must register the callback
 * function for each message type. If no callback function is registered the
 * received message is ignored. Additionally the user can provide a data
 * context pointer for each callback function that will be forwarded with any
 * call to that function.
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

#ifndef ENDPOINTTARGETDETECTION_H_INCLUDED
#define ENDPOINTTARGETDETECTION_H_INCLUDED

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/
#include <stdint.h>
#include "EndpointRadarErrorCodes.h"

/* Enable C linkage if header is included in C++ files */
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * \defgroup EndpointTargetDetection EndpointTargetDetection
 *
* \brief EndpointTargetDetection documentation.
 *
 * @{
 */

/*
==============================================================================
   2. DEFINITIONS
==============================================================================
*/

/*
==============================================================================
   3. TYPES
==============================================================================
*/

/**
 *
 * \brief Data structure for DSP settings for the target detection processing.
 *
 */
typedef struct
{
    uint8_t  range_mvg_avg_length;		/**< Moving average filter length used for range */
	
	uint16_t min_range_cm;				/**< Minimum range below which targets are ignored */
    uint16_t max_range_cm;				/**< Maximum range above which targets are ignored */
    uint16_t min_speed_kmh;				/**< Minimum speed below which targets are ignored */
    uint16_t max_speed_kmh;				/**< Maximum speed above which targets are ignored */    
    uint16_t min_angle_degree;			/**< Minimum angle below which targets are ignored.
                                         	 Not supported yet */
    uint16_t max_angle_degree;          /**< Maximum angle above which targets are ignored.
                                         	 Not supported yet */
	uint16_t range_threshold;			/**< Range FFT linear threshold below which targets
											 are ignored */
    uint16_t speed_threshold;			/**< Doppler FFT linear threshold below which targets
											 are ignored */
	
	uint8_t  enable_tracking;			/**< Enable / Disable tracking */
	uint8_t  num_of_tracks;				/**< Number of active tracks */
	
	uint8_t  median_filter_length;	    /**< Depth of median filter used to smooth the angle values */
	uint8_t  enable_mti_filter;         /**< Enable / Disable MTI filter to remove static targets */
	
	uint16_t mti_filter_length;			/**< Length of MTI filter in terms of frame count after which 
	                                         static target should be killed */

} DSP_Settings_t;

/**
 *
 * \brief Data structure for transfer of target parameters.
 *
 */
typedef struct
{
    uint32_t target_id;        /**< An unique ID of that target. */
	
    float    level;            /**< The Level at the peak in dB relative to
                                    threshold */
    float    radius;           /**< The Distance of the target from the
                                    sensor */
    float    azimuth;          /**< The azimuth angle of the target. Positive
                                    values in right direction from the sensing
                                    board perspective */
    float    elevation;        /**< The elevation angle of the target.
                                    Positive values in up direction from the
                                    sensing board perspective */
    float    radial_speed;     /**< The change of radius per second. */
	
    float    azimuth_speed;    /**< The change of azimuth angle per second */
	
    float    elevation_speed;  /**< The change of elevation angle per second */
	
} Target_Info_t;


// ---------------------------------------------------------------------------------

/**
* \brief This functions registers a callback for algorithm DSP settings defined by 
* \ref DSP_Settings_t.
*
* If a callback for this message type is registered, that callback is issued
* every time, a connected board requests current DSP settings from device using 
* \ref ep_targetdetect_get_dsp_settings().If no callback is registered 
* the message is ignored.
*
* Whenever a callback is issued, the following parameters are passed to the callback:
*
* \param[in] context          The context data pointer, provided along with
*                             the callback itself through
*                             \ref ep_targetdetect_set_callback_dsp_settings.
* \param[in] protocol_handle  The handle of the connection, the sending
*                             device is connected to.
* \param[in] endpoint         The number of the endpoint that has sent the
*                             message.
* \param[in] dsp_settings_ptr Current DSP settings used by algorithm on target is returned
*                             into this structure.
*
*/
typedef void(*Callback_Dsp_Settings_t)(void* context,
                                       int32_t protocol_handle,
                                       uint8_t endpoint,
                                       const DSP_Settings_t* dsp_settings_ptr);

/**
* \brief This functions registers a callback to get current target list calculated by 
* algorithm on target. This target list is defined by \ref Target_Info_t.
*
* If a callback for this message type is registered, that callback is issued
* every time, a connected board requests current target list calculated from device using 
* \ref ep_targetdetect_get_targets().If no callback is registered 
* the message is ignored.
*
* Whenever a callback is issued, the following parameters are passed to the callback:
*
* \param[in] context          The context data pointer, provided along with
*                             the callback itself through
*                             \ref ep_targetdetect_set_callback_target_processing.
* \param[in] protocol_handle  The handle of the connection, the sending
*                             device is connected to.
* \param[in] endpoint         The number of the endpoint that has sent the
*                             message.
* \param[in] targets          Recent target list calculated by algorithm on target is passed 
*                             into this structure.
*
*/									   
typedef void(*Callback_Target_Processing_t)(void* context,
                                            int32_t protocol_handle,
                                            uint8_t endpoint,
                                            const Target_Info_t* targets,
                                            uint8_t num_targets);

/**
* \brief This functions registers a callback to get range FFT threshold in 
* linear scale used by algorithm on target.
*
* If a callback for this message type is registered, that callback is issued
* every time, a connected board requests current range FFT threshold from device using 
* \ref ep_targetdetect_get_range_threshold().If no callback is registered 
* the message is ignored.
*
* Whenever a callback is issued, the following parameters are passed to the callback:
*
* \param[in] context          The context data pointer, provided along with
*                             the callback itself through
*                             \ref ep_targetdetect_set_callback_range_threshold.
* \param[in] protocol_handle  The handle of the connection, the sending
*                             device is connected to.
* \param[in] endpoint         The number of the endpoint that has sent the
*                             message.
* \param[in] threshold        Current range FFT threshold (in linear scale) used by 
*                             algorithm on target.
*
*/											
typedef void(*Callback_Range_Threshold_t)(void* context,
                                          int32_t protocol_handle,
                                          uint8_t endpoint,
                                          uint8_t threshold);

/*
==============================================================================
   5. FUNCTION PROTOTYPES AND INLINE FUNCTIONS
==============================================================================
*/

/**
 * \brief This function checks if an endpoint in a device is an TargetDetect
 *        endpoint.
 *
 * This function checks type and version of the specified endpoint in a
 * connected device and returns a code that indicates if that endpoint is
 * compatible to the radar endpoint implementation in this module.
 * 
 * \param[in] protocol_handle  A handle to the connection to the sensor
 *                             device.
 * \param[in] endpoint         The endpoint in the connected device to be
 *                             checked for compatibility.
 *
 * \return If the specified endpoint is compatible to this implementation the
 *         function returns 0. If the endpoint is not compatible, a negative
 *         error code is returned.
 */
int32_t ep_targetdetect_is_compatible_endpoint(int32_t protocol_handle,
                                               uint8_t endpoint);

/**
 * \brief This functions registers a callback to get radar DSP setting.
 *
 * For more information about the callback function see \ref Callback_Dsp_Settings_t.
 *
 * \param[in] callback  The function to be called when a radar DSP settings
 *                      structure is received.
 * \param[in] context   A data pointer that is forwarded to the callback
 *                      function.
 */
void ep_targetdetect_set_callback_dsp_settings(Callback_Dsp_Settings_t callback,
                                               void* context);

/**
 * \brief This functions registers a callback to get target list for detected targets.
 *
 * For more information about the callback function see \ref Callback_Target_Processing_t.
 *
 * \param[in] callback  The function to be called when a radar target list 
 *                      is received.
 * \param[in] context   A data pointer that is forwarded to the callback
 *                      function.
 */											   
void ep_targetdetect_set_callback_target_processing(Callback_Target_Processing_t callback,
                                                    void* context);

/**
 * \brief This functions registers a callback to get recent range threshold used by algorithm.
 *
 * For more information about the callback function see \ref Callback_Range_Threshold_t.
 *
 * \param[in] callback  The function to be called when a range threshold  
 *                      is received.
 * \param[in] context   A data pointer that is forwarded to the callback
 *                      function.
 */														
void ep_targetdetect_set_callback_range_threshold(Callback_Range_Threshold_t callback,
                                                   void* context);

//============================================================================
												   
/**
 * \brief This function sets the DSP settings for the algorithm used in the device.
 *
 * DSP settings is defined by struct \ref DSP_Settings_t.
 * 
 * \param[in] protocol_handle  A handle to the connection to the sensor
 *                             device.
 * \param[in] endpoint         The endpoint in the connected device to be
 *                             checked for compatibility.
 * \param[in] pDSPSettings     Pointer to a DSP structure \ref DSP_Settings_t to be set 
 *                             in the algorithm used in the device.
 * \return If the specified endpoint is compatible to this implementation the
 *         function returns 0. If the endpoint is not compatible, a negative
 *         error code is returned.
 */
int32_t ep_targetdetect_set_dsp_settings(int32_t protocol_handle,
                                         uint8_t endpoint,
                                         const DSP_Settings_t* pDSPSettings);
/**
 * \brief This function gets the DSP settings for the algorithm used in the device.
 *
 * DSP settings is defined by struct \ref DSP_Settings_t. This DSP structure is returned 
 * via callback which user has register defined by \ref Callback_Dsp_Settings_t.
 * 
 * \param[in] protocol_handle  A handle to the connection to the sensor
 *                             device.
 * \param[in] endpoint         The endpoint in the connected device to be
 *                             checked for compatibility.
 * \return If the specified endpoint is compatible to this implementation the
 *         function returns 0. If the endpoint is not compatible, a negative
 *         error code is returned.
 */	 
int32_t ep_targetdetect_get_dsp_settings(int32_t protocol_handle,
                                         uint8_t endpoint);
										 
/**
 * \brief This function gets the range threshold used by the algorithm in the device.
 *
 * This threshold is returned via callback which user has register defined by 
 * \ref Callback_Range_Threshold_t.
 * 
 * \param[in] protocol_handle  A handle to the connection to the sensor
 *                             device.
 * \param[in] endpoint         The endpoint in the connected device to be
 *                             checked for compatibility.
 * \return If the specified endpoint is compatible to this implementation the
 *         function returns 0. If the endpoint is not compatible, a negative
 *         error code is returned.
 */	 
int32_t ep_targetdetect_get_range_threshold(int32_t protocol_handle,
                                         uint8_t endpoint);										 
	
/**
 * \brief This function gets the target list calculated by the algorithm used in the device.
 *
 * This target list is returned via callback which user has register defined by 
 * \ref Callback_Target_Processing_t.
 * 
 * \param[in] protocol_handle  A handle to the connection to the sensor
 *                             device.
 * \param[in] endpoint         The endpoint in the connected device to be
 *                             checked for compatibility.
 * \return If the specified endpoint is compatible to this implementation the
 *         function returns 0. If the endpoint is not compatible, a negative
 *         error code is returned.
 */ 
int32_t ep_targetdetect_get_targets(int32_t protocol_handle, uint8_t endpoint);


/** @} */

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

/* End of include guard */
#endif /* ENDPOINTTARGETDETECTION_H_INCLUDED */

/* --- End of File -------------------------------------------------------- */
