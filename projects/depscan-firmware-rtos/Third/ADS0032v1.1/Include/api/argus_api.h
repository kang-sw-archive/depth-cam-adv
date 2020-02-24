/*************************************************************************//**
 * @file
 * @brief    	This file is part of the AFBR-S50 API.
 * @details		This file provides generic functionality belonging to all
 * 				devices from the AFBR-S50 product family.
 * 
 * @copyright	Copyright c 2016-2019, Avago Technologies GmbH.
 * 				All rights reserved.
 *****************************************************************************/

#ifndef ARGUS_API_H
#define ARGUS_API_H

/*!***************************************************************************
 * @defgroup	argusapi AFBR-S50 API
 *
 * @brief		The main module of the API from the AFBR-S50 SDK.
 *
 * @details		General API for the AFBR-S50 time-of-flight sensor device family.
 *
 * @addtogroup 	argusapi
 * @{
 *****************************************************************************/

#include "argus_def.h"
#include "argus_res.h"
#include "argus_pba.h"

/*! The data structure for the API representing a AFBR-S50 device instance. */
typedef void argus_hnd_t;

/*! The S2PI slave identifier. */
typedef int32_t s2pi_slave_t;

/*!***************************************************************************
 * @brief 	Initializes the API modules and the device with default parameters.
 *
 * @details The function that needs to be called once after power up to
 * 			initialize the modules state (i.e. the corresponding handle) and the
 * 			dedicated Time-of-Flight device. In order to obtain a handle,
 * 			reference the #Argus_CreateHandle method.
 *
 * 			Prior to calling the function, the required peripherals (i.e. S2PI,
 * 			GPIO w/ IRQ and Timers) must be initialized and ready to use.
 *
 * 			The function executes the following tasks:
 * 			- Initialization of the internal state represented by the handle
 * 			  object.
 * 			- Setup the device such that an safe configuration is present in
 * 			  the registers.
 * 			- Initialize sub modules such as calibration or measurement modules.
 * 			.
 *
 * 			The modules configuration is initialized with reasonable default values.
 *
 * @param	hnd The API handle; contains all internal states and data.
 *
 * @param	spi_slave The SPI hardware slave, i.e. the specified CS and IRQ
 * 						lines. This is actually just a number that is passed
 * 						to the SPI interface to distinct for multiple SPI slave
 * 						devices. Note that the slave must be not equal to 0,
 * 						since is reserved for error handling.
 *
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_Init(argus_hnd_t * hnd, s2pi_slave_t spi_slave);

/*!***************************************************************************
 * @brief 	Reinitializes the API modules and the device with default parameters.
 *
 * @details The function reinitializes the device with default configuration.
 * 			Can be used as reset sequence for the device. See #Argus_Init for
 * 			more information on the initialization.
 *
 * 			Note that the #Argus_Init function must be called first! Otherwise,
 * 			the function will return an error if it is called for an yet
 * 			uninitialized device/handle.
 *
 * @param	hnd The API handle; contains all internal states and data.
 *
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_Reinit(argus_hnd_t * hnd);

/*!***************************************************************************
 * @brief 	Creates a new device data handle object to store all internal states.
 *
 * @details The function must be called to obtain a new device handle object.
 * 			The handle is basically an abstract object in memory that contains
 * 			all the internal states and settings of the API module. The handle
 * 			is passed to all the API methods in order to address the specified
 * 			device. This allows to use the API with more than a single measurement
 * 			device.
 *
 * 			The handler is created by calling the memory allocation method from
 * 			the standard library: @code void * malloc(size_t size) @endcode
 * 			In order to implement an individual memory allocation method,
 * 			define and implement the following weakly binded method and return
 * 			a pointer to the newly allocated memory. *
 * 			@code void * Argus_Malloc (size_t size) @endcode
 * 			Also see the #Argus_DestroyHandle method for the corresponding
 * 			deallocation of the allocated memory.
 *
 * @return 	Returns a pointer to the newly allocated device handler object.
 * 			Returns a null pointer if the allocation failed!
 *****************************************************************************/
argus_hnd_t * Argus_CreateHandle(void);

/*!***************************************************************************
 * @brief 	Destroys a given device data handle object.
 *
 * @details The function can be called to free the previously created device
 * 			data handle object in order to save memory when the device is not
 * 			used any more.
 *
 * 			Please refer to the #Argus_CreateHandle method for the corresponding
 * 			allocation of the memory.
 *
 * 			The handler is destroyed by freeing the corresponding memory with the
 * 			method from the standard library, @code void free(void * ptr) @endcode.
 * 			In order to implement an individual memory deallocation method, define
 * 			and implement the following weakly binded method and free the memory
 * 			object passed to the method by a pointer.
 *
 * 			@code void Argus_Free (void * ptr) @endcode
 *
 * @param	hnd The device handle object to be deallocated.
 *****************************************************************************/
void Argus_DestroyHandle(argus_hnd_t * hnd);

/*!**************************************************************************
 * Generic API
 ****************************************************************************/

/*!***************************************************************************
 * @brief	Gets the version number of the current API library.
 *
 * @return 	Returns the current version number.
 *****************************************************************************/
uint32_t Argus_GetAPIVersion(void);

/*!***************************************************************************
 * @brief	Gets the version number of the chip.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @return 	Returns the current version number.
 *****************************************************************************/
uint32_t Argus_GetChipVersion(argus_hnd_t * hnd);

/*!***************************************************************************
 * @brief	Gets the version/variant of the module.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @return 	Returns the current module number.
 *****************************************************************************/
argus_module_version_t Argus_GetModuleVersion(argus_hnd_t * hnd);

/*!***************************************************************************
 * @brief	Gets the unique identification number of the chip.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @return 	Returns the unique identification number.
 *****************************************************************************/
uint32_t Argus_GetChipID(argus_hnd_t * hnd);

/*!***************************************************************************
 * @brief	Gets the SPI hardware slave identifier.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @return	The SPI hardware slave identifier.
 *****************************************************************************/
s2pi_slave_t Argus_GetSPISlave(argus_hnd_t * hnd);

/*! @} */

/*!**************************************************************************
 * Measurement/Device Operation
 ****************************************************************************
 * @addtogroup 	argusmeas
 * @{
 ****************************************************************************/

/*!***************************************************************************
 * @brief	Starts the timer based measurement cycle asynchronously.
 *
 * @details This function starts a timer based measurement cycle asynchronously.
 * 			in the background. A periodic timer interrupt triggers the measurement
 * 			frames on the ASIC and the data readout afterwards. When the frame is
 * 			finished, a callback (which is passed as a parameter to the function)
 * 			is invoked in order to inform the main thread to call the \link
 * 			#Argus_EvaluateData data evaluation method\endlink. This call is
 * 			mandatory to release the data buffer for the next measurement cycle
 * 			and it must not be invoked from the callback since it is within an
 * 			interrupt service routine. Rather a flag should inform the main thread
 * 			to invoke the evaluation as soon as possible in order to not introduce
 * 			any unwanted delays to the next measurement frame.
 *			The next measurement frame will be started as soon as the pre-
 *			conditions are meet. These are:
 *			 1. timer flag set (i.e. a certain time has passed since the last
 *			 	measurement in order to fulfill eye-safety),
 *			 2. device idle (i.e. no measurement currently ongoing) and
 *			 3. data buffer ready (i.e. the previous data has been evaluated).
 *			Usually, the device idle and data buffer ready conditions are met
 *			before the timer tick occurs and thus the timer dictates the frame
 *			rate.
 *
 * 			The callback function pointer will be invoked when the measurement
 * 			frame has finished successfully or whenever an error, that cannot
 * 			be handled internally, occurs.
 *
 *			The periodic timer interrupts are used to check the measurement status
 *			for timeouts. An error is invoked when a measurement cycle have not
 *			finished within the specified time.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	cb  Callback function that will be invoked when the measurement
 * 				  is completed. Its parameters are the \link #status_t status
 * 				  \endlink and a pointer to the \link #argus_results_t results
 * 				  \endlink structure. If an error occurred, the status differs
 * 				  from #STATUS_OK and the second parameter is null.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_StartMeasurementTimer(argus_hnd_t * hnd, argus_callback_t cb);

/*!***************************************************************************
 * @brief	Stops the timer based measurement cycle.
 *
 * @details This function stops the ongoing timer based measurement cycles.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_StopMeasurementTimer(argus_hnd_t * hnd);

/*!***************************************************************************
 * @brief	Triggers a single measurement frame asynchronously.
 *
 * @details This function immediately triggers a single measurement frame
 * 			asynchronously if all the pre-conditions are met. Otherwise it returns
 * 			with a corresponding status.
 *			When the frame is finished, a callback (which is passed as a parameter
 *			to the function) is invoked in order to inform the main thread to
 *			call the \link #Argus_EvaluateData data evaluation method\endlink.
 *			This call is mandatory to release the data buffer for the next
 *			measurement and it must not be invoked from the callback since it is
 *			within an interrupt service routine. Rather a flag should inform
 *			the main thread to invoke the evaluation.
 *			The pre-conditions for starting a measurement frame are:
 *			 1. timer flag set (i.e. a certain time has passed since the last
 *			 	measurement in order to fulfill eye-safety),
 *			 2. device idle (i.e. no measurement currently ongoing) and
 *			 3. data buffer ready (i.e. the previous data has been evaluated).
 *
 * 			The callback function pointer will be invoked when the measurement
 * 			frame has finished successfully or whenever an error, that cannot
 * 			be handled internally, occurs.
 *
 *			The successful finishing of the measurement frame is not checked
 *			for timeouts! Instead, the user can call the #Argus_GetStatus()
 *			function to do so.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	cb	Callback function that will be invoked when the measurement
 * 				is completed. Its parameters are the \link #status_t status
 * 				\endlink and a pointer to the \link #argus_results_t results
 * 				\endlink structure. If an error occurred, the status differs
 * 				from #STATUS_OK and the second parameter is null.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_TriggerMeasurement(argus_hnd_t * hnd, argus_callback_t cb);

/*!***************************************************************************
 * @brief 	Stops the currently ongoing measurements and SPI activity immediately.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_Abort(argus_hnd_t * hnd);

/*!***************************************************************************
 * @brief 	Checks the state of the device/driver.
 *
 * @details	Returns the current module state:
 *			- Idle:  Device and SPI interface are idle (== #STATUS_OK).
 * 			- Busy:  Device or SPI interface are busy (== #STATUS_BUSY).
 * 			- Timeout: A previous frame measurement has not finished within a
 * 					   specified time (== #ERROR_TIMEOUT).
 * 			- Fail:  Some problems occurred (== #ERROR_FAIL).
 * 			- (Error: Another error has occurred.)
 *			.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetStatus(argus_hnd_t * hnd);

/*!*****************************************************************************
 * @brief 	Tests the connection to the device by sending a ping message.
 *
 * @details	A ping is transfered to the device in order to check the device and
 * 			SPI connection status. Returns #STATUS_OK on success and
 * 			#ERROR_ARGUS_NOT_CONNECTED elsewise.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 ******************************************************************************/
status_t Argus_Ping(argus_hnd_t * hnd);

/*!***************************************************************************
 * @brief 	Evaluate useful information from the raw measurement data.
 *
 * @details	This function is called with a pointer to the raw results obtained
 * 			from the measurement cycle. It evaluates this data and creates
 * 			useful information from it. Furthermore, calibration is applied to
 * 			the data. Finally, the results are used in order to adapt the device
 * 			configuration to the ambient conditions in order to achieve optimal
 * 			device performance.\n
 * 			Therefore, it consists of the following sub-functions:
 * 			- Apply pre-calibration: Applies calibration steps before evaluating
 * 			  the data, i.e. calculations that are to the integration results
 * 			  directly.
 * 			- Evaluate data: Calculates measurement parameters such as range,
 * 			  amplitude or ambient light intensity, depending on the configurations.
 * 			- Apply post-calibration: Applies calibrations after evaluation of
 * 			  measurement data, i.e. calibrations applied to the calculated
 * 			  values such as range.
 * 			- Dynamic Configuration Adaption: checks if the configuration needs
 *			  to be adjusted before the next measurement cycle in order to
 *			  achieve optimum performance. Note that the configuration might not
 *			  applied directly but before the next measurement starts. This is
 *			  due to the fact that the device could be busy measuring already
 *			  the next frame and thus no SPI activity is allowed.
 *			.
 *			However, if the device is idle, the configuration will be written
 *			immediately.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	res A pointer to the results structure that will be populated
 * 				  with evaluated data.
 * @param	raw The pointer to the raw data that has been obtained by the
 * 				  measurement finished callback.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_EvaluateData(argus_hnd_t * hnd, argus_results_t * res, void * raw);

/*!***************************************************************************
 * @brief	Executes a substrate voltage calibration measurement.
 *
 * @details This function immediately triggers a calibration measurement
 * 			sequence. The ordinary measurement activity is suspended while the
 * 			calibration is ongoing.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_ExecuteVsubCalibrationSequence(argus_hnd_t * hnd);

/*!***************************************************************************
 * @brief	Executes a crosstalk calibration measurement.
 *
 * @details This function immediately triggers a crosstalk vector calibration
 * 			measurement sequence. The ordinary measurement activity is suspended
 * 			while the calibration is ongoing.
 *
 * 			In order to perform a crosstalk calibration, the reflection of the
 * 			transmitted signal must be kept from the receiver side, by either
 * 			covering the TX completely (or RX respectively) or by setting up
 * 			an absorbing target at far distance.
 *
 * 			After calibration has finished successfully, the obtained data is
 * 			applied immediately and can be read from the API using the
 * 			#Argus_GetCalibrationCrosstalkVectorTable function.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_ExecuteXtalkCalibrationSequence(argus_hnd_t * hnd, argus_mode_t mode);


/*! @} */

/*!**************************************************************************
 * Configuration API
 ****************************************************************************
 * @addtogroup 	arguscfg
 * @{
 ****************************************************************************/

/*!***************************************************************************
 * @brief 	Sets the measurement mode to a specified device.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	value The new measurement mode.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationMeasurementMode(argus_hnd_t * hnd,
											   argus_mode_t value);

/*!***************************************************************************
 * @brief 	Gets the measurement mode from a specified device.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	value The current measurement mode.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationMeasurementMode(argus_hnd_t * hnd,
											   argus_mode_t * value);

/*!***************************************************************************
 * @brief 	Sets the frame time to a specified device.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	value The measurement frame time in microseconds.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationFrameTime(argus_hnd_t * hnd, uint32_t value);

/*!***************************************************************************
 * @brief	Gets the frame time from a specified device.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	value The current frame time in microseconds.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationFrameTime(argus_hnd_t * hnd, uint32_t * value);

/*!***************************************************************************
 * @brief 	Sets the amplitude checker configuration values to a specified device.
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	amplitude The new amplitude threshold in UQ12.4 format.
 * @param	timeout The new failure timeout in milliseconds.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationAmplitudeChecker(argus_hnd_t * hnd,
												argus_mode_t mode,
												uq12_4_t amplitude,
												uint16_t timeout);

/*!***************************************************************************
 * @brief 	Gets the amplitude checker configuration values from a specified device.
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	amplitude The current amplitude threshold in UQ12.4 format.
 * @param	timeout The current failure timeout in milliseconds.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationAmplitudeChecker(argus_hnd_t * hnd,
												argus_mode_t mode,
												uq12_4_t* amplitude,
												uint16_t * timeout);

/*!***************************************************************************
 * @brief 	Sets the integration timeout margin to a specified device.
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The new integration timeout margin in UQ0.8 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationIntegrationTimeout(argus_hnd_t * hnd,
												  argus_mode_t mode,
												  uq0_8_t value);

/*!***************************************************************************
 * @brief 	Gets the integration timeout margin from a specified device.
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The current integration timeout margin in UQ0.8 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationIntegrationTimeout(argus_hnd_t * hnd,
												  argus_mode_t mode,
												  uq0_8_t * value);

/*!***************************************************************************
 * @brief 	Sets the dual frequency mode (DFM) enabled flag to a specified device.
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The new DFM enabled flag.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDFMEnabled(argus_hnd_t * hnd,
										  argus_mode_t mode,
										  bool value);

/*!***************************************************************************
 * @brief 	Gets the dual frequency mode (DFM) enabled flag from a specified device.
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The current DFM enabled flag.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDFMEnabled(argus_hnd_t * hnd,
										  argus_mode_t mode,
										  bool * value);

/*!***************************************************************************
 * @brief 	Sets the full DCA module configuration to a specified device.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The new DCA configuration set.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDynamicAdaption(argus_hnd_t * hnd,
											   argus_mode_t mode,
											   argus_cfg_dca_t const * value);

/*!***************************************************************************
 * @brief 	Gets the # from a specified device.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The current DCA configuration set value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDynamicAdaption(argus_hnd_t * hnd,
											   argus_mode_t mode,
											   argus_cfg_dca_t * value);

/*!***************************************************************************
 * @brief 	Sets the DCA enabled to a specified device.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The new DCA enabled value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDCAEnabled(argus_hnd_t * hnd,
										  argus_mode_t mode,
										  argus_dca_enable_t value);

/*!***************************************************************************
 * @brief 	Gets the DCA enabled from a specified device.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The current DCA enabled value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDCAEnabled(argus_hnd_t * hnd,
										  argus_mode_t mode,
										  argus_dca_enable_t * value);

/*!***************************************************************************
 * @brief 	Sets the DCA saturated pixel threshold count for linear response
 * 			to a specified device.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The new DCA linear threshold value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDCALinThreshold(argus_hnd_t * hnd,
											   argus_mode_t mode,
											   uint8_t value);

/*!***************************************************************************
 * @brief 	Gets the DCA saturated pixel threshold count for linear response
 * 			from a specified device.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The current DCA linear threshold value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDCALinThreshold(argus_hnd_t * hnd,
											   argus_mode_t mode,
											   uint8_t * value);

/*!***************************************************************************
 * @brief 	Sets the DCA saturated pixel threshold count for exponential response
 * 			to a specified device.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The new DCA exponential threshold value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDCAExpThreshold(argus_hnd_t * hnd,
											   argus_mode_t mode,
											   uint8_t value);

/*!***************************************************************************
 * @brief 	Gets the DCA saturated pixel threshold count for exponential response
 * 			from a specified device.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The current DCA exponential threshold value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDCAExpThreshold(argus_hnd_t * hnd,
											   argus_mode_t mode,
											   uint8_t * value);

/*!***************************************************************************
 * @brief 	Sets the DCA saturated pixel threshold count for reset response
 * 			to a specified device.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The new DCA reset threshold value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDCAResetThreshold(argus_hnd_t * hnd,
												 argus_mode_t mode,
												 uint8_t value);

/*!***************************************************************************
 * @brief 	Gets the DCA saturated pixel threshold count for reset response
 * 			from a specified device.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The current DCA reset threshold value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDCAResetThreshold(argus_hnd_t * hnd,
												 argus_mode_t mode,
												 uint8_t * value);

/*!***************************************************************************
 * @brief 	Sets the DCA target amplitude to a specified device.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The new DCA target amplitude in UQ12.4 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDCATargetAmplitude(argus_hnd_t * hnd,
												  argus_mode_t mode,
												  uq12_4_t value);

/*!***************************************************************************
 * @brief 	Gets the DCA target amplitude from a specified device.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The current DCA target amplitude in UQ12.4 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDCATargetAmplitude(argus_hnd_t * hnd,
												  argus_mode_t mode,
												  uq12_4_t * value);

/*!***************************************************************************
 * @brief 	Sets the DCA low threshold amplitude to a specified device.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The new DCA low threshold amplitude in UQ12.4 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDCALowAmplitude(argus_hnd_t * hnd,
											   argus_mode_t mode,
											   uq12_4_t value);

/*!***************************************************************************
 * @brief 	Gets the DCA low threshold amplitude from a specified device.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The current DCA low threshold amplitude in UQ12.4 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDCALowAmplitude(argus_hnd_t * hnd,
											   argus_mode_t mode,
											   uq12_4_t * value);

/*!***************************************************************************
 * @brief 	Sets the DCA high threshold amplitude to a specified device.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The new DCA high threshold amplitude in UQ12.4 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDCAHighAmplitude(argus_hnd_t * hnd,
												argus_mode_t mode,
												uq12_4_t value);

/*!***************************************************************************
 * @brief 	Gets the DCA high threshold amplitude from a specified device.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The current DCA high threshold amplitude in UQ12.4 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDCAHighAmplitude(argus_hnd_t * hnd,
												argus_mode_t mode,
												uq12_4_t * value);

/*!***************************************************************************
 * @brief 	Sets the DCA nominal integration depth to a specified device.
 *
 * @details	The analog integration depth determines the time of analog signal
 * 			correlation measured in counts of laser pattern. Note that also
 * 			fractional patterns can be sent and thus the analog integration
 * 			depth may be a fractional value. Therefore its format is an UQ10.6
 * 			fixed point number.
 *
 * 			The nominal analog integration depth determines the static or start
 * 			value for the adjustment range of the DCA algorithm.
 * 			Note that if the DCA is disabled, the nominal value is used as
 * 			a static setting.
 *
 * 			In order to get the actual analog integration depth of a given
 * 			measurement data set, use the corresponding measurement data API
 * 			methods.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The new DCA minimum integration depth value in UQ10.6 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDCANomIntegrationDepth(argus_hnd_t * hnd,
													  argus_mode_t mode,
													  uq10_6_t value);

/*!***************************************************************************
 * @brief 	Gets the DCA nominal integration depth from a specified device.
 *
 * @details	The analog integration depth determines the time of analog signal
 * 			correlation measured in counts of laser pattern. Note that also
 * 			fractional patterns can be sent and thus the analog integration
 * 			depth may be a fractional value. Therefore its format is an UQ10.6
 * 			fixed point number.
 *
 * 			The nominal analog integration depth determines the static or start
 * 			value for the adjustment range of the DCA algorithm.
 * 			Note that if the DCA is disabled, the nominal value is used as
 * 			a static setting.
 *
 * 			In order to get the actual analog integration depth of a given
 * 			measurement data set, use the corresponding measurement data API
 * 			methods.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The new DCA minimum integration depth value in UQ10.6 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDCANomIntegrationDepth(argus_hnd_t * hnd,
													  argus_mode_t mode,
													  uq10_6_t * value);

/*!***************************************************************************
 * @brief 	Sets the DCA minimum integration depth to a specified device.
 *
 * @details	The analog integration depth determines the time of analog signal
 * 			correlation measured in counts of laser pattern. Note that also
 * 			fractional patterns can be sent and thus the analog integration
 * 			depth may be a fractional value. Therefore its format is an UQ10.6
 * 			fixed point number.
 *
 * 			The minimum analog integration depth determines the lower boundary
 * 			for the adjustment range of the DCA algorithm.
 * 			Note that if the DCA is enabled, the static setting is overwritten
 * 			by the dynamic internal setting.
 *
 * 			In order to get the actual analog integration depth of a given
 * 			measurement data set, use the corresponding measurement data API
 * 			methods.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The new DCA minimum integration depth value in UQ10.6 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDCAMinIntegrationDepth(argus_hnd_t * hnd,
													  argus_mode_t mode,
													  uq10_6_t value);

/*!***************************************************************************
 * @brief 	Gets the DCA minimum integration depth from a specified device.
 *
 * @details	The analog integration depth determines the time of analog signal
 * 			correlation measured in counts of laser pattern. Note that also
 * 			fractional patterns can be sent and thus the analog integration
 * 			depth may be a fractional value. Therefore its format is an UQ10.6
 * 			fixed point number.
 *
 * 			The minimum analog integration depth determines the lower boundary
 * 			for the adjustment range of the DCA algorithm.
 * 			Note that if the DCA is enabled, the static setting is overwritten
 * 			by the dynamic internal setting.
 *
 * 			In order to get the actual analog integration depth of a given
 * 			measurement data set, use the corresponding measurement data API
 * 			methods.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The new DCA minimum integration depth value in UQ10.6 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDCAMinIntegrationDepth(argus_hnd_t * hnd,
													  argus_mode_t mode,
													  uq10_6_t * value);

/*!***************************************************************************
 * @brief 	Sets the DCA maximum integration depth to a specified device.
 *
 * @details	The analog integration depth determines the time of analog signal
 * 			correlation measured in counts of laser pattern. Note that also
 * 			fractional patterns can be sent and thus the analog integration
 * 			depth may be a fractional value. Therefore its format is an UQ10.6
 * 			fixed point number.
 *
 * 			The maximum analog integration depth determines the upper boundary
 * 			for the adjustment range of the DCA algorithm.
 * 			Note that if the DCA is enabled, the static setting is overwritten
 * 			by the dynamic internal setting.
 *
 * 			In order to get the actual analog integration depth of a given
 * 			measurement data set, use the corresponding measurement data API
 * 			methods.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The new DCA maximum integration depth value in UQ10.6 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDCAMaxIntegrationDepth(argus_hnd_t * hnd,
													  argus_mode_t mode,
													  uq10_6_t value);

/*!***************************************************************************
 * @brief 	Gets the DCA maximum integration depth from a specified device.
 *
 * @details	The analog integration depth determines the time of analog signal
 * 			correlation measured in counts of laser pattern. Note that also
 * 			fractional patterns can be sent and thus the analog integration
 * 			depth may be a fractional value. Therefore its format is an UQ10.6
 * 			fixed point number.
 *
 * 			The maximum analog integration depth determines the upper boundary
 * 			for the adjustment range of the DCA algorithm.
 * 			Note that if the DCA is enabled, the static setting is overwritten
 * 			by the dynamic internal setting.
 *
 * 			In order to get the actual analog integration depth of a given
 * 			measurement data set, use the corresponding measurement data API
 * 			methods.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The new DCA maximum integration depth value in UQ10.6 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDCAMaxIntegrationDepth(argus_hnd_t * hnd,
													  argus_mode_t mode,
													  uq10_6_t * value);

/*!***************************************************************************
 * @brief 	Sets the DCA nominal/maximal optical power to a specified device.
 *
 * @details	The laser modulation power (aka OMA, optical modulation amplitude)
 * 			is determined by the laser driver modulation current. Therefore,
 * 			the value is specified in mA by a UQ12.4 format number.
 *
 * 			The nominal output power determines the the static or start value
 * 			for the adjustment range of the DCA algorithm. It also determines
 * 			the maximum value used for the DCA algorithm.
 * 			Note that if the DCA is disabled, the nominal value is used as
 * 			a static setting.
 *
 * 			In order to get the actual analog integration depth of a given
 * 			measurement data set, use the corresponding measurement data API
 * 			methods.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The new DCA minimum output power in UQ12.4 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDCANomOutputPower(argus_hnd_t * hnd,
												 argus_mode_t mode,
												 uq12_4_t value);

/*!***************************************************************************
 * @brief 	Gets the DCA nominal/maximal output power from a specified device.
 *
 * @details	The laser modulation power (aka OMA, optical modulation amplitude)
 * 			is determined by the laser driver modulation current. Therefore,
 * 			the value is specified in mA by a UQ12.4 format number.
 *
 * 			The nominal output power determines the the static or start value
 * 			for the adjustment range of the DCA algorithm. It also determines
 * 			the maximum value used for the DCA algorithm.
 * 			Note that if the DCA is disabled, the nominal value is used as
 * 			a static setting.
 *
 * 			In order to get the actual analog integration depth of a given
 * 			measurement data set, use the corresponding measurement data API
 * 			methods.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The current DCA minimum output power in UQ12.4 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDCANomOutputPower(argus_hnd_t * hnd,
												 argus_mode_t mode,
												 uq12_4_t * value);

/*!***************************************************************************
 * @brief 	Sets the DCA minimum optical power to a specified device.
 *
 * @details	The laser modulation power (aka OMA, optical modulation amplitude)
 * 			is determined by the laser driver modulation current. Therefore,
 * 			the value is specified in mA by a UQ12.4 format number.
 *
 * 			The minimum output power determines the lower boundary for the
 * 			adjustment range of the DCA algorithm.
 * 			Note that if the DCA is enabled, the static setting is overwritten
 * 			by the dynamic internal setting.
 *
 * 			In order to get the actual analog integration depth of a given
 * 			measurement data set, use the corresponding measurement data API
 * 			methods.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The new DCA minimum output power in UQ12.4 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDCAMinOutputPower(argus_hnd_t * hnd,
												 argus_mode_t mode,
												 uq12_4_t value);

/*!***************************************************************************
 * @brief 	Gets the DCA minimum output power from a specified device.
 *
 * @details	The laser modulation power (aka OMA, optical modulation amplitude)
 * 			is determined by the laser driver modulation current. Therefore,
 * 			the value is specified in mA by a UQ12.4 format number.
 *
 * 			The minimum output power determines the lower boundary for the
 * 			adjustment range of the DCA algorithm.
 * 			Note that if the DCA is enabled, the static setting is overwritten
 * 			by the dynamic internal setting.
 *
 * 			In order to get the actual analog integration depth of a given
 * 			measurement data set, use the corresponding measurement data API
 * 			methods.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The current DCA minimum output power in UQ12.4 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDCAMinOutputPower(argus_hnd_t * hnd,
												 argus_mode_t mode,
												 uq12_4_t * value);

/*!***************************************************************************
 * @brief 	Sets the DCA nominal gain stage value to a specified device.
 *
 * @details	The pixel input gain is determined by the charge pump voltage.
 * 			There are up to four stages available:
 *
 * 			- #DCA_GAIN_LOW: approx. gain = 12.5
 * 			- #DCA_GAIN_MEDIUM_LOW: approx. gain = 25
 * 			- #DCA_GAIN_MEDIUM_HIGH: approx. gain = 50
 * 			- #DCA_GAIN_HIGH: approx. gain = 100
 *			.
 *
 *			The actual gain depends on device and its individual calibration.
 *
 * 			The nominal gain stage determines a mode with the common gain that
 * 			is used as the common gain value. This is used by the DCA to operate
 * 			in the normal regime.
 *
 * 			Note that if the DCA is enabled, the static setting is overwritten
 * 			by the dynamic internal setting.
 *
 * 			In order to get the actual pixel gain stage of a given measurement
 * 			data set, use the corresponding measurement data API methods.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The new DCA nominal gain stage value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDCANomPixelGain(argus_hnd_t * hnd,
											   argus_mode_t mode,
											   argus_dca_gain_t value);

/*!***************************************************************************
 * @brief 	Gets the DCA nominal gain stage value from a specified device.
 *
 * @details	The pixel input gain is determined by the charge pump voltage.
 * 			There are up to four stages available:
 *
 * 			- #DCA_GAIN_LOW: approx. gain = 12.5
 * 			- #DCA_GAIN_MEDIUM_LOW: approx. gain = 25
 * 			- #DCA_GAIN_MEDIUM_HIGH: approx. gain = 50
 * 			- #DCA_GAIN_HIGH: approx. gain = 100
 *			.
 *
 *			The actual gain depends on device and its individual calibration.
 *
 * 			The nominal gain stage determines a mode with the common gain that
 * 			is used as the common gain value. This is used by the DCA to operate
 * 			in the normal regime.
 *
 * 			Note that if the DCA is enabled, the static setting is overwritten
 * 			by the dynamic internal setting.
 *
 * 			In order to get the actual pixel gain stage of a given measurement
 * 			data set, use the corresponding measurement data API methods.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The current DCA nominal gain stage value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDCANomPixelGain(argus_hnd_t * hnd,
											   argus_mode_t mode,
											   argus_dca_gain_t * value);

/*!***************************************************************************
 * @brief 	Sets the DCA minimal gain stage value to a specified device.
 *
 * @details	The pixel input gain is determined by the charge pump voltage.
 * 			There are up to four stages available:
 *
 * 			- #DCA_GAIN_LOW: approx. gain = 12.5
 * 			- #DCA_GAIN_MEDIUM_LOW: approx. gain = 25
 * 			- #DCA_GAIN_MEDIUM_HIGH: approx. gain = 50
 * 			- #DCA_GAIN_HIGH: approx. gain = 100
 *			.
 *
 *			The actual gain depends on device and its individual calibration.
 *
 * 			The minimal gain stage determines a mode with the lowest gain that
 * 			is used for strong signals. This is used by the DCA to operate in
 * 			the low gain regime.
 *
 * 			Note that if the DCA is enabled, the static setting is overwritten
 * 			by the dynamic internal setting.
 *
 * 			In order to get the actual pixel gain stage of a given measurement
 * 			data set, use the corresponding measurement data API methods.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The new DCA minimum gain stage value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDCAMinPixelGain(argus_hnd_t * hnd,
											   argus_mode_t mode,
											   argus_dca_gain_t value);

/*!***************************************************************************
 * @brief 	Sets the DCA minimal gain stage value to a specified device.
 *
 * @details	The pixel input gain is determined by the charge pump voltage.
 * 			There are up to four stages available:
 *
 * 			- #DCA_GAIN_LOW: approx. gain = 12.5
 * 			- #DCA_GAIN_MEDIUM_LOW: approx. gain = 25
 * 			- #DCA_GAIN_MEDIUM_HIGH: approx. gain = 50
 * 			- #DCA_GAIN_HIGH: approx. gain = 100
 *			.
 *
 *			The actual gain depends on device and its individual calibration.
 *
 * 			The minimal gain stage determines a mode with the lowest gain that
 * 			is used for strong signals. This is used by the DCA to operate in
 * 			the low gain regime.
 *
 * 			Note that if the DCA is enabled, the static setting is overwritten
 * 			by the dynamic internal setting.
 *
 * 			In order to get the actual pixel gain stage of a given measurement
 * 			data set, use the corresponding measurement data API methods.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The new DCA minimum gain stage value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDCAMinPixelGain(argus_hnd_t * hnd,
											   argus_mode_t mode,
											   argus_dca_gain_t * value);

/*!***************************************************************************
 * @brief 	Sets the DCA nominal gain stage value to a specified device.
 *
 * @details	The pixel input gain is determined by the charge pump voltage.
 * 			There are up to four stages available:
 *
 * 			- #DCA_GAIN_LOW: approx. gain = 12.5
 * 			- #DCA_GAIN_MEDIUM_LOW: approx. gain = 25
 * 			- #DCA_GAIN_MEDIUM_HIGH: approx. gain = 50
 * 			- #DCA_GAIN_HIGH: approx. gain = 100
 *			.
 *
 *			The actual gain depends on device and its individual calibration.
 *
 * 			The maximum gain stage determines a mode with the highest gain that
 * 			is used for weak signals. This is used by the DCA to operate in the
 * 			high gain regime.
 *
 * 			Note that if the DCA is enabled, the static setting is overwritten
 * 			by the dynamic internal setting.
 *
 * 			In order to get the actual pixel gain stage of a given measurement
 * 			data set, use the corresponding measurement data API methods.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The new DCA maximum gain stage value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDCAMaxPixelGain(argus_hnd_t * hnd,
											   argus_mode_t mode,
											   argus_dca_gain_t value);

/*!***************************************************************************
 * @brief 	Gets the DCA nominal gain stage value from a specified device.
 *
 * @details	The pixel input gain is determined by the charge pump voltage.
 * 			There are up to four stages available:
 *
 * 			- #DCA_GAIN_LOW: approx. gain = 12.5
 * 			- #DCA_GAIN_MEDIUM_LOW: approx. gain = 25
 * 			- #DCA_GAIN_MEDIUM_HIGH: approx. gain = 50
 * 			- #DCA_GAIN_HIGH: approx. gain = 100
 *			.
 *
 *			The actual gain depends on device and its individual calibration.
 *
 * 			The maximum gain stage determines a mode with the highest gain that
 * 			is used for weak signals. This is used by the DCA to operate in the
 * 			high gain regime.
 *
 * 			Note that if the DCA is enabled, the static setting is overwritten
 * 			by the dynamic internal setting.
 *
 * 			In order to get the actual pixel gain stage of a given measurement
 * 			data set, use the corresponding measurement data API methods.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The current DCA maximum gain stage value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDCAMaxPixelGain(argus_hnd_t * hnd,
											   argus_mode_t mode,
											   argus_dca_gain_t * value);

/*!***************************************************************************
 * @brief 	Sets the DCA power saving ratio to a specified device.
 *
 * @details	Determines the percentage of the full available frame time that is
 * 			not exploited for digital integration. Thus the device is idle
 * 			within the specified portion of the frame time and does consume
 * 			less energy.
 *
 * 			Note that the laser safety might already limit the maximum
 * 			integration depth and the power saving ratio might not take effect
 * 			for all ambient situations. Thus the Power Saving Ratio is to be
 * 			understood as a minimum percentage where the device is idle per
 * 			frame.
 *
 * 			The value is a UQ0.8 format that ranges from 0.0 (=0x00) to 0.996
 * 			(=0xFF), where 0 means no power saving (i.e. feature disabled) and
 * 			0xFF determines maximum power saving, i.e. the digital integration
 * 			depth is limited to a single sample.
 *
 * 			Range: 0x00, .., 0xFF; set 0 to disable.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The new DCA power saving ratio value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDCAPowerSavingRatio(argus_hnd_t * hnd,
												   argus_mode_t mode,
												   uq0_8_t value);

/*!***************************************************************************
 * @brief 	Gets the DCA power saving ratio from a specified device.
 *
 * @details	Determines the percentage of the full available frame time that is
 * 			not exploited for digital integration. Thus the device is idle
 * 			within the specified portion of the frame time and does consume
 * 			less energy.
 *
 * 			Note that the laser safety might already limit the maximum
 * 			integration depth and the power saving ratio might not take effect
 * 			for all ambient situations. Thus the Power Saving Ratio is to be
 * 			understood as a minimum percentage where the device is idle per
 * 			frame.
 *
 * 			The value is a UQ0.8 format that ranges from 0.0 (=0x00) to 0.996
 * 			(=0xFF), where 0 means no power saving (i.e. feature disabled) and
 * 			0xFF determines maximum power saving, i.e. the digital integration
 * 			depth is limited to a single sample.
 *
 * 			Range: 0x00, .., 0xFF; set 0 to disable.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The current DCA power saving ratio value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDCAPowerSavingRatio(argus_hnd_t * hnd,
												   argus_mode_t mode,
												   uq0_8_t * value);

/*!***************************************************************************
 * @brief 	Sets the pixel binning configuration parameters to a specified device.
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The new pixel binning configuration parameters.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationPixelBinning(argus_hnd_t * hnd,
											argus_mode_t mode,
											argus_cfg_pba_t const * value);

/*!***************************************************************************
 * @brief 	Gets the pixel binning configuration parameters from a specified device.
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The current pixel binning configuration parameters.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationPixelBinning(argus_hnd_t * hnd,
											argus_mode_t mode,
											argus_cfg_pba_t * value);

/*!***************************************************************************
 * @brief 	Sets the PBA enabled flag to a specified device.
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The new pixel binning enabled flag.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationPBAEnabled(argus_hnd_t * hnd,
										  argus_mode_t mode,
										  bool value);

/*!***************************************************************************
 * @brief 	Gets the PBA enabled flag from a specified device.
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The current pixel binning enabled flag.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationPBAEnabled(argus_hnd_t * hnd,
										  argus_mode_t mode,
										  bool * value);

/*!***************************************************************************
 * @brief 	Sets the PBA averaging mode to a specified device.
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The new pixel binning averaging mode.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationPBAAveragingMode(argus_hnd_t * hnd,
												argus_mode_t mode,
												argus_pba_averaging_mode_t value);

/*!***************************************************************************
 * @brief 	Gets the PBA enabled flag from a specified device.
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The current pixel binning averaging mode.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationPBAAveragingMode(argus_hnd_t * hnd,
												argus_mode_t mode,
												argus_pba_averaging_mode_t * value);

/*!***************************************************************************
 * @brief 	Sets the PBA pre-filter pixel mask to a specified device.
 * @details	The pre-filter pixel mask determines the pixel channels that are
 * 			statically excluded from the pixel binning (i.e. 1D distance) result.
 *
 * 			The pixel enabled mask is an 32-bit mask that determines the
 * 			device internal channel number. It is recommended to use the
 * 			 - #PIXELXY_ISENABLED(msk, x, y)
 * 			 - #PIXELXY_ENABLE(msk, x, y)
 * 			 - #PIXELXY_DISABLE(msk, x, y)
 * 			 .
 * 			macros to work with the pixel enable masks.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The new pre-filter pixel mask.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationPBAPrefilterPixelMask(argus_hnd_t * hnd,
													 argus_mode_t mode,
													 uint32_t value);

/*!***************************************************************************
 * @brief 	Gets the PBA pre-filter pixel mask from a specified device.
 * @details	The pre-filter pixel mask determines the pixel channels that are
 * 			statically excluded from the pixel binning (i.e. 1D distance) result.
 *
 * 			The pixel enabled mask is an 32-bit mask that determines the
 * 			device internal channel number. It is recommended to use the
 * 			 - #PIXELXY_ISENABLED(msk, x, y)
 * 			 - #PIXELXY_ENABLE(msk, x, y)
 * 			 - #PIXELXY_DISABLE(msk, x, y)
 * 			 .
 * 			macros to work with the pixel enable masks.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The current pre-filter pixel mask.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationPBAPrefilterPixelMask(argus_hnd_t * hnd,
													 argus_mode_t mode,
													 uint32_t * value);

/*!***************************************************************************
 * @brief 	Sets the PBA pre-filter pixel(x,y) enabled value to a specified device.
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	x The x-coordinate of the specified pixel.
 * @param	y The y-coordinate of the specified pixel.
 * @param	value The new pre-filter pixel(x,y) enabled value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationPBAPrefilterPixel(argus_hnd_t * hnd,
												 argus_mode_t mode,
												 uint8_t x,
												 uint8_t y,
												 bool value);

/*!***************************************************************************
 * @brief 	Gets the PBA pre-filter pixel(x,y) enabled value from a specified device.
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	x The x-coordinate of the specified pixel.
 * @param	y The y-coordinate of the specified pixel.
 * @param	value The current pre-filter pixel(x,y) enabled value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationPBAPrefilterPixel(argus_hnd_t * hnd,
												 argus_mode_t mode,
												 uint8_t x,
												 uint8_t y,
												 bool * value);

/*!***************************************************************************
 * @brief 	Sets the PBA relative amplitude threshold value to a specified device.
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The new PBA relative amplitude threshold value in UQ0.8 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationPBARelAmplitudeThreshold(argus_hnd_t * hnd,
														argus_mode_t mode,
														uq0_8_t value);

/*!***************************************************************************
 * @brief 	Gets the PBA relative amplitude threshold value from a specified device.
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The current PBA relative amplitude threshold value in UQ0.8 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationPBARelAmplitudeThreshold(argus_hnd_t * hnd,
														argus_mode_t mode,
														uq0_8_t * value);

/*!***************************************************************************
 * @brief 	Sets the PBA absolute amplitude threshold value to a specified device.
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The new PBA absolute amplitude threshold value in UQ12.4 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationPBAAbsAmplitudeThreshold(argus_hnd_t * hnd,
														argus_mode_t mode,
														uq12_4_t value);

/*!***************************************************************************
 * @brief 	Gets the PBA absolute amplitude threshold value from a specified device.
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The current PBA absolute amplitude threshold value in UQ12.4 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationPBAAbsAmplitudeThreshold(argus_hnd_t * hnd,
														argus_mode_t mode,
														uq12_4_t * value);

/*!***************************************************************************
 * @brief 	Sets the PBA minimum distance scope enabled flag to a specified device.
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The new pixel binning minimum distance scope enabled flag.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationPBAMinDistanceScopeEnabled(argus_hnd_t * hnd,
														  argus_mode_t mode,
														  bool value);

/*!***************************************************************************
 * @brief 	Gets the PBA minium distance scope enabled flag from a specified device.
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The current pixel binning minimum distance scope enabled flag.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationPBAMinDistanceScopeEnabled(argus_hnd_t * hnd,
														  argus_mode_t mode,
														  bool * value);

/*!***************************************************************************
 * @brief 	Sets the PBA relative minimum distance scope threshold value to a
 * 			specified device.
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The new PBA relative minimum distance scope threshold value
 * 					in UQ0.8 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationPBARelMinDistanceScope(argus_hnd_t * hnd,
													  argus_mode_t mode,
													  uq0_8_t value);

/*!***************************************************************************
 * @brief 	Gets the PBA relative minimum distance scope threshold value from
 * 			a specified device.
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The current PBA relative minimum distance scope threshold
 * 					value in UQ0.8 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationPBARelMinDistanceScope(argus_hnd_t * hnd,
													  argus_mode_t mode,
													  uq0_8_t * value);

/*!***************************************************************************
 * @brief 	Sets the PBA absolute minimum distance scope threshold value to a
 * 			specified device.
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The new PBA absolute minimum distance scope threshold value
 * 					in meter and UQ1.15 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationPBAAbsMinDistanceScope(argus_hnd_t * hnd,
													  argus_mode_t mode,
													  uq1_15_t value);

/*!***************************************************************************
 * @brief 	Gets the PBA absolute minimum distance scope threshold value from
 * 			a specified device.
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The current PBA absolute minimum distance scope threshold
 * 					value in meter and UQ1.15 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationPBAAbsMinDistanceScope(argus_hnd_t * hnd,
													  argus_mode_t mode,
													  uq1_15_t * value);

/*!***************************************************************************
 * @brief 	Sets the PBA golden pixel enabled flag to a specified device.
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The new pixel binning golden pixel enabled flag.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationPBAGoldenPixelEnabled(argus_hnd_t * hnd,
													 argus_mode_t mode,
													 bool value);

/*!***************************************************************************
 * @brief 	Gets the PBA golden pixel enabled flag from a specified device.
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The current pixel binning golden pixel enabled flag.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationPBAGoldenPixelEnabled(argus_hnd_t * hnd,
													 argus_mode_t mode,
													 bool * value);

/*!***************************************************************************
 * @brief 	Gets the current unambiguous range in mm.
 * @param	hnd The API handle; contains all internal states and data.
 * @param	range_mm The returned range in mm.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationUnambiguousRange(argus_hnd_t * hnd,
												uint32_t * range_mm);

/*! @} */

/*!**************************************************************************
 * Calibration API
 ****************************************************************************
 * @addtogroup 	arguscal
 * @{
 ****************************************************************************/

/*!***************************************************************************
 * @brief 	Sets the global range offset value to a specified device.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The new global range offset in meter and Q9.22 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetCalibrationGlobalRangeOffset(argus_hnd_t * hnd,
											   argus_mode_t mode,
											   q9_22_t value);

/*!***************************************************************************
 * @brief 	Gets the global range offset value from a specified device.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The current global range offset in meter and Q9.22 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetCalibrationGlobalRangeOffset(argus_hnd_t * hnd,
											   argus_mode_t mode,
											   q9_22_t * value);

/*!***************************************************************************
 * @brief 	Sets the custom crosstalk vector table to a specified device.
 *
 * @details The crosstalk vectors are subtracted from the raw sampling data
 * 			in the data evaluation phase.
 *
 * 			The crosstalk vector table is a three dimensional array  of type
 * 			#xtalk_t.
 *
 * 			The dimensions are:
 * 			 - size(0) = #ARGUS_DFM_FRAME_COUNT (Dual-frequency mode A- or B-frame)
 * 			 - size(1) = #ARGUS_PIXELS_X (Pixel count in x-direction)
 * 			 - size(2) = #ARGUS_PIXELS_Y (Pixel count in y-direction)
 *			 .
 *
 * 			Its recommended to use the built-in crosstalk calibration sequence
 * 			(see #Argus_ExecuteXtalkCalibrationSequence) to determine the
 * 			crosstalk vector table.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The new crosstalk vector table.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetCalibrationCrosstalkVectorTable(argus_hnd_t * hnd,
												  argus_mode_t mode,
												  xtalk_t value[ARGUS_DFM_FRAME_COUNT][ARGUS_PIXELS_X][ARGUS_PIXELS_Y]);

/*!***************************************************************************
 * @brief 	Gets the custom crosstalk vector table from a specified device.
 *
 * @details The crosstalk vectors are subtracted from the raw sampling data
 * 			in the data evaluation phase.
 *
 * 			The crosstalk vector table is a three dimensional array  of type
 * 			#xtalk_t.
 *
 * 			The dimensions are:
 * 			 - size(0) = #ARGUS_DFM_FRAME_COUNT (Dual-frequency mode A- or B-frame)
 * 			 - size(1) = #ARGUS_PIXELS_X (Pixel count in x-direction)
 * 			 - size(2) = #ARGUS_PIXELS_Y (Pixel count in y-direction)
 *			 .
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The current crosstalk vector table.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetCalibrationCrosstalkVectorTable(argus_hnd_t * hnd,
												  argus_mode_t mode,
												  xtalk_t value[ARGUS_DFM_FRAME_COUNT][ARGUS_PIXELS_X][ARGUS_PIXELS_Y]);

/*!***************************************************************************
 * @brief 	Gets the factory calibrated default crosstalk vector table for the
 * 			specified device.
 *
 * @details The crosstalk vectors are subtracted from the raw sampling data
 * 			in the data evaluation phase.
 *
 * 			The crosstalk vector table is a three dimensional array  of type
 * 			#xtalk_t.
 *
 * 			The dimensions are:
 * 			 - size(0) = #ARGUS_DFM_FRAME_COUNT (Dual-frequency mode A- or B-frame)
 * 			 - size(1) = #ARGUS_PIXELS_X (Pixel count in x-direction)
 * 			 - size(2) = #ARGUS_PIXELS_Y (Pixel count in y-direction)
 *			 .
 *
 * 			The total vector table consists of the custom crosstalk vector
 * 			table (set via #Argus_SetCalibrationCrosstalkVectorTable) and
 * 			an internal, factory calibrated device specific vector table.
 * 			This is informational only!
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @param	value The current total crosstalk vector table.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetCalibrationTotalCrosstalkVectorTable(argus_hnd_t * hnd,
													   argus_mode_t mode,
													   xtalk_t value[ARGUS_DFM_FRAME_COUNT][ARGUS_PIXELS_X][ARGUS_PIXELS_Y]);

/*!***************************************************************************
 * @brief 	Resets the crosstalk vector table for the specified device to the
 * 			factory calibrated default values.
 *
 * @details The crosstalk vectors are subtracted from the raw sampling data
 * 			in the data evaluation phase.
 * *
 * 			The factory defaults are device specific calibrated values.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	mode The targeted measurement mode.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_ResetCalibrationCrosstalkVectorTable(argus_hnd_t * hnd,
													argus_mode_t mode);

/*!***************************************************************************
 * @brief 	Sets the sample count for the crosstalk calibration sequence.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	value The new crosstalk calibration sequence sample count.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetCalibrationCrosstalkSequenceSampleCount(argus_hnd_t * hnd,
														  uint16_t value);

/*!***************************************************************************
 * @brief 	Gets the sample count for the crosstalk calibration sequence.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	value The current crosstalk calibration sequence sample count.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetCalibrationCrosstalkSequenceSampleCount(argus_hnd_t * hnd,
														  uint16_t * value);

/*!***************************************************************************
 * @brief 	Sets the max. amplitude threshold for the crosstalk calibration sequence.
 *
 * @details	The maximum amplitude threshold defines a maximum crosstalk vector
 * 			amplitude before causing an error message. If the crosstalk is
 * 			too high, there is usually an issue with the measurement setup, i.e.
 * 			there is still a measurement signal detected.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	value The new crosstalk calibration sequence maximum amplitude
 * 					threshold value in UQ12.4 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetCalibrationCrosstalkSequenceAmplitudeThreshold(argus_hnd_t * hnd,
																 uq12_4_t value);

/*!***************************************************************************
 * @brief 	Gets the max. amplitude threshold for the crosstalk calibration sequence.
 *
 * @details	The maximum amplitude threshold defines a maximum crosstalk vector
 * 			amplitude before causing an error message. If the crosstalk is
 * 			too high, there is usually an issue with the measurement setup, i.e.
 * 			there is still a measurement signal detected.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	value The current max. amplitude threshold value in UQ12.4 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetCalibrationCrosstalkSequenceAmplitudeThreshold(argus_hnd_t * hnd,
																 uq12_4_t * value);

/*!***************************************************************************
 * @brief 	Sets the sample count for the substrate voltage calibration sequence.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	value The new substrate voltage calibration sequence sample count.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetCalibrationVsubSequenceSampleCount(argus_hnd_t * hnd,
													 uint16_t value);

/*!***************************************************************************
 * @brief 	Gets the sample count for the substrate voltage calibration sequence.
 *
 * @param	hnd The API handle; contains all internal states and data.
 * @param	value The current substrate voltage calibration sequence sample count.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetCalibrationVsubSequenceSampleCount(argus_hnd_t * hnd,
													 uint16_t * value);

/*!***************************************************************************
 * @brief 	A callback that returns the external crosstalk vector table.
 *
 * @details The function needs to be implemented by the host application in
 * 			order to set the external crosstalk vector table upon system
 * 			initialization. If not defined in user code, the default
 * 			implementation will return an all zero vector table, assuming there
 * 			is no (additional) external crosstalk.
 *
 * 			If defined in user code, the function must fill all vector values
 * 			in the provided \par xtalk parameter with external crosstalk values.
 * 			The values can be obtained by the calibration routine.
 *
 *			Example usage:
 *
 *			@code
 *			status_t Argus_GetExternalCrosstalkVectorTable_Callback(xtalk_t xtalk[ARGUS_PIXELS_X][ARGUS_PIXELS_Y],
 *																	argus_mode_t mode, argus_module_version_t module)
 *			{
 *				(void) mode; // Ignore mode; use same values for all modes.
 *				(void) module; // Ignore module; use same values for all modules.
 *				memset(&xtalk, 0, sizeof(xtalk));
 *
 *				// Set crosstalk vectors in Q11.4 format.
 *				xtalk[0][0][0].dS = -9;		xtalk[0][0][0].dC = -11;
 *				xtalk[0][0][1].dS = -13;	xtalk[0][0][1].dC = -16;
 *				xtalk[0][0][2].dS = 6;		xtalk[0][0][2].dC = -18;
 *				// etc.
 *			}
 *			@endcode
 *
 * @param	xtalk The crosstalk vector array; to be filled with data.
 * @param	mode Determines the current measurement mode; can be ignored if
 * 				   only a single measurement mode is utilized.
 *****************************************************************************/
void Argus_GetExternalCrosstalkVectorTable_Callback(xtalk_t xtalk[ARGUS_DFM_FRAME_COUNT][ARGUS_PIXELS_X][ARGUS_PIXELS_Y],
													argus_mode_t mode);


/*! @} */
#endif /* ARGUS_API_H */
