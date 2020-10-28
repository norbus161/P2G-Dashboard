/**
 * \file EndPointCalibration.h
 *
 * \brief This file defines the API to communicate with Radar Calibration Endpoints
 *        in Infineon 24GHz devices.
 *
 * The module EndPointCalibration.c provides functions to communicate with
 * the radar calibration endpoint in Infineon 24GHz devices. The user must 
 * use the function provided by the module \ref Protocol.c to open a connection 
 * to a radar sensor device and query information about the endpoints in the device.
 *
 * The function \ref ep_radar_calibration_is_compatible_endpoint can be used to check
 * each endpoint if it is compatible with this API. If it is the types and
 * functions in from this module can be used to send parameters to the
 * endpoint and query the current dsp settings and radar target information.
 *
 * All data received from the connected device is returned through callbacks.
 * For each message type that can be received from the device, a separate
 * callback function type is defined. The user must register the callback
 * function for each message type. If no callback function is registered the
 * received message is ignored. Additionally the user can provide a data
 * context pointer for each callback function that will be forwarded with any
 * call to that function.
 */


#ifndef ENDPOINTCALIBRATION_H_INCLUDED
#define ENDPOINTCALIBRATION_H_INCLUDED

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
 * \defgroup EndpointCalibration EndpointCalibration
 *
* \brief EndpointCalibraton documentation.
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
 * \brief This structure holds information about algorithm calibration data.
 *
 * Hardware modules could have offset in the angle because of some mismatches in 
 * antennas. To compensate this constant offset in angle, algorithm calibration needs 
 * to store the angle offset value in angle_offset_deg field. 
 * Similarly, there is fixed delay between TX to RX path which reflects into an offset in range. 
 * This can be removed using algorithm calibration field distance_offset_cm.
 *
 */
typedef struct
{
	uint16_t 	distance_offset_cm;		/**< Distance offset in units of cm */
	int16_t		angle_offset_deg;		/**< Angle offset in units of degrees */

} Algo_Calibrations_t;										   


/**
* \brief This functions registers a callback for raw ADC calibration data.
*
* If a callback for this message type is registered, that callback is issued
* every time, a connected board requests raw ADC calibration data from device using 
* \ref ep_calibration_get_adc_flash_calibration_data() or 
* \ref ep_calibration_get_adc_sram_calibration_data().If no callback is registered 
* the message is ignored.
*
* Whenever a callback is issued, the following parameters are passed to the callback:
*
* \param[in] context          The context data pointer, provided along with
*                             the callback itself through
*                             \ref ep_calibration_set_callback_adc_calibration_data.
* \param[in] protocol_handle  The handle of the connection, the sending
*                             device is connected to.
* \param[in] endpoint         The number of the endpoint that has sent the
*                             message.
* \param[in] calibration_data_ptr*  The radar ADC calibration data from FLASH or SRAM 
*                                   is stored in this memory location.
* \param[in] num_of_samples    Number of 16-bit samples received from device as calibration data.
*
*/
typedef void(*Callback_ADC_Calibration_Data_t)(void* context,
											int32_t protocol_handle,
											uint8_t endpoint,
											const float* calibration_data_ptr,
											uint16_t num_of_samples);

/**
* \brief This functions registers a callback for algorithm offsets correction related calibration data.
*
* If a callback for this message type is registered, that callback is issued
* every time, a connected board requests algorithm calibration data from device using 
* \ref ep_calibration_get_algo_flash_calibration_data() or 
* \ref ep_calibration_get_algo_sram_calibration_data().
*
* If no callback is registered the message is ignored.
*
* Whenever a callback is issued, the following parameters are passed to the callback:
*
* \param[in] context          The context data pointer, provided along with
*                             the callback itself through
*                             \ref ep_calibration_set_callback_algo_calibration_data.
* \param[in] protocol_handle  The handle of the connection, the sending
*                             device is connected to.
* \param[in] endpoint         The number of the endpoint that has sent the
*                             message.
* \param[in] algo_calibration_data_ptr*  Struct defined by \ref Algo_Calibrations_t.
*
*/											
typedef void(*Callback_Algo_Calibration_Data_t)(void* context,
											int32_t protocol_handle,
											uint8_t endpoint,
											const Algo_Calibrations_t* algo_calibration_data_ptr);										   
										   
/*
==============================================================================
   5. FUNCTION PROTOTYPES AND INLINE FUNCTIONS
==============================================================================
*/

/**
* \brief This function checks if an endpoint in a device is Calibration
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
int32_t ep_calibration_is_compatible_endpoint(int32_t protocol_handle,
											uint8_t endpoint);

/**
* \brief This functions registers a callback for radar ADC calibration data messages.
*
* If a callback for this message type is registered, that callback is issued
* every time, a connected board sends a message containing radar calibration data.
* If no callback is registered the message is ignored.
*
* Connection and handle and endpoint number of the sending endpoint are
* passed to the callback along with the message data. Furthermore the pointer
* context set by this function is forwarded to the callback.
*
* \param[in] callback  The function to be called when a radar calibration data
*                      message is received.
* \param[in] context   A data pointer that is forwarded to the callback
*                      function.
*/
void ep_calibration_set_callback_adc_calibration_data(Callback_ADC_Calibration_Data_t callback,
													void* context);
												  
/**
* \brief This functions registers a callback for radar algorithm calibration data messages.
*
* If a callback for this message type is registered, that callback is issued
* every time, a connected board sends a message containing radar calibration data.
* If no callback is registered the message is ignored.
*
* Connection and handle and endpoint number of the sending endpoint are
* passed to the callback along with the message data. Furthermore the pointer
* context set by this function is forwarded to the callback.
*
* \param[in] callback  The function to be called when a radar calibration data
*                      message is received.
* \param[in] context   A data pointer that is forwarded to the callback
*                      function.
*/												  
void ep_calibration_set_callback_algo_calibration_data(Callback_Algo_Calibration_Data_t callback,
													void* context);												  

//============================================================================

/**
 * \brief This function sets ADC calibration data in Flash memory of the device.
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
int32_t ep_calibration_set_adc_flash_calibration_data(int32_t protocol_handle,
													uint8_t endpoint);
																								   
/**
 * \brief This function sets ADC calibration data in SRAM memory of the device.
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
int32_t ep_calibration_set_adc_sram_calibration_data(int32_t protocol_handle,
													uint8_t endpoint);													

/**
 * \brief This function gets ADC calibration data from the Flash memory of the device.
 * 
 * Calibration data is returned via a callback defined by \ref Callback_ADC_Calibration_Data_t.
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
int32_t ep_calibration_get_adc_flash_calibration_data(int32_t protocol_handle,
													uint8_t endpoint);

/**
 * \brief This function gets ADC calibration data from the SRAM of the device.
 * 
 * Calibration data is returned via a callback defined by \ref Callback_ADC_Calibration_Data_t.
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
int32_t ep_calibration_get_adc_sram_calibration_data(int32_t protocol_handle,
													uint8_t endpoint);													

/**
 * \brief This function clears ADC calibration data from the Flash memory of the device.
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
int32_t ep_calibration_clear_adc_flash_calibration_data(int32_t protocol_handle,
													uint8_t endpoint);

/**
 * \brief This function clears ADC calibration data from the SRAM of the device.
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
int32_t ep_calibration_clear_adc_sram_calibration_data(int32_t protocol_handle,
													uint8_t endpoint);
											   
//============================================================================

/**
 * \brief This function sets algorithm calibration data in Flash memory of the device.
 * 
 * Algorithm calibrations are because of hardware issues. Algorithm calibrations are 
 * defined by \ref Algo_Calibrations_t.
 *
 * \param[in] protocol_handle  A handle to the connection to the sensor
 *                             device.
 * \param[in] endpoint         The endpoint in the connected device to be
 *                             checked for compatibility.
 * \param[in] algo_cal_data    Pointer to struct defined by \ref Algo_Calibrations_t
 *
 * \return If the specified endpoint is compatible to this implementation the
 *         function returns 0. If the endpoint is not compatible, a negative
 *         error code is returned.
 */
int32_t ep_calibration_set_algo_flash_calibration_data(int32_t protocol_handle,
													uint8_t endpoint,
													const Algo_Calibrations_t* algo_cal_data);

/**
 * \brief This function sets algorithm calibration data in SRAM of the device.
 * 
 * Algorithm calibrations are because of hardware issues. Algorithm calibrations are 
 * defined by \ref Algo_Calibrations_t.
 *
 * \param[in] protocol_handle  A handle to the connection to the sensor
 *                             device.
 * \param[in] endpoint         The endpoint in the connected device to be
 *                             checked for compatibility.
 * \param[in] algo_cal_data    Pointer to struct defined by \ref Algo_Calibrations_t
 *
 * \return If the specified endpoint is compatible to this implementation the
 *         function returns 0. If the endpoint is not compatible, a negative
 *         error code is returned.
 */													
int32_t ep_calibration_set_algo_sram_calibration_data(int32_t protocol_handle,
													uint8_t endpoint,
													const Algo_Calibrations_t* algo_cal_data);

/**
 * \brief This function gets algorithm calibration data in Flash memory of the device.
 * 
 * Algorithm calibrations are returned via a callback \ref Callback_Algo_Calibration_Data_t.
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
int32_t ep_calibration_get_algo_flash_calibration_data(int32_t protocol_handle,
													uint8_t endpoint);
													
/**
 * \brief This function gets algorithm calibration data in SRAM of the device.
 * 
 * Algorithm calibrations are returned via a callback \ref Callback_Algo_Calibration_Data_t.
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
int32_t ep_calibration_get_algo_sram_calibration_data(int32_t protocol_handle,
													uint8_t endpoint);													

/**
 * \brief This function clears algorithm calibration data in Flash memory of the device.
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
int32_t ep_calibration_clear_algo_flash_calibration_data(int32_t protocol_handle,
													uint8_t endpoint);											  

/**
 * \brief This function clears algorithm calibration data in SRAM of the device.
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
int32_t ep_calibration_clear_algo_sram_calibration_data(int32_t protocol_handle,
													uint8_t endpoint);											   

/** @} */

/* --- Close open blocks -------------------------------------------------- */

/* Disable C linkage for C++ files */
#ifdef __cplusplus
}  /* extern "C" */
#endif /* __cplusplus */

/* End of include guard */
#endif /* ENDPOINT_CALIBRATION_H_INCLUDED */

/* --- End of File -------------------------------------------------------- */
