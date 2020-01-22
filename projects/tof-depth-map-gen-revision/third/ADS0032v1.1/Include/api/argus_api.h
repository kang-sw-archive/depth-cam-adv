/*************************************************************************//**
 * @file
 * @brief    	This file is part of the Argus hardware API.
 * @details		This file provides generic functionality belonging to all
 * 				devices from the Argus product family.
 * 
 * @copyright	Copyright c 2016-2018, Avago Technologies GmbH.
 * 				All rights reserved.
 *****************************************************************************/

#ifndef ARGUS_API_H
#define ARGUS_API_H

/*!***************************************************************************
 * @defgroup	argusapi Argus API
 * @brief		The main module of the Argus API from the AFBR-S50 SDK.
 * @details		Generalizes the API functions for all Argus devices.
 * @addtogroup 	argusapi
 * @{
 *****************************************************************************/

#include "argus_def.h"
#include "argus_res.h"
#include "argus_cal.h"
#include "argus_pba.h"

/*! The data structure for the Argus ToF API. */
typedef struct Argus_Handle argus_hnd_t;

/*! The S2PI slave identifier. */
typedef int32_t s2pi_slave_t;

/*!***************************************************************************
 * @brief 	Initialized the API modules and the device with default parameters.
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
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	spi_slave : The SPI hardware slave, i.e. the specified CS and IRQ lines.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_Init(argus_hnd_t * hnd, s2pi_slave_t spi_slave);

/*!***************************************************************************
 * @brief 	Creates a new device data handle object to store all internal states.
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
 * @param	hnd : The device handle object to be deallocated.
 * @return 	-
 *****************************************************************************/
void Argus_DestroyHandle(argus_hnd_t * hnd);

/*!***************************************************************************
 * @brief	Starts the timer based measurement cycle asynchronously.
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
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param 	cb  : Callback function that will be invoked when the measurement
 * 				  is completed. Its parameters are the \link #status_t status
 * 				  \endlink and a pointer to the \link #argus_results_t results
 * 				  \endlink structure. If an error occurred, the status differs
 * 				  from #STATUS_OK and the second parameter is null.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_StartMeasurementTimer(argus_hnd_t * hnd, argus_callback_t cb);

/*!***************************************************************************
 * @brief	Stops the timer based measurement cycle.
 * @details This function stops the ongoing timer based measurement cycles.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_StopMeasurementTimer(argus_hnd_t * hnd);

/*!***************************************************************************
 * @brief	Triggers a single measurement frame asynchronously.
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
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param 	cb  : Callback function that will be invoked when the measurement
 * 				  is completed. Its parameters are the \link #status_t status
 * 				  \endlink and a pointer to the \link #argus_results_t results
 * 				  \endlink structure. If an error occurred, the status differs
 * 				  from #STATUS_OK and the second parameter is null.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_TriggerMeasurement(argus_hnd_t * hnd, argus_callback_t cb);

/*!***************************************************************************
 * @brief 	Stops the currently ongoing measurements and SPI activity immediately.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_Abort(argus_hnd_t * hnd);

/*!***************************************************************************
 * @brief 	Checks the state of the device/driver.
 * @details	Returns the current module state:
 *			- Idle:  Device and SPI interface are idle (== #STATUS_OK).
 * 			- Busy:  Device or SPI interface are busy (== #STATUS_BUSY).
 * 			- Timeout: A previous frame measurement has not finished within a
 * 					   specified time (== #ERROR_TIMEOUT).
 * 			- Fail:  Some problems occurred (== #ERROR_FAIL).
 * 			- (Error: Another error has occurred.)
 *			.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetStatus(argus_hnd_t * hnd);

/*!*****************************************************************************
 * @brief 	Tests the connection to the device by sending a ping message.
 * @details	A ping is transfered to the device in order to check the device and
 * 			SPI connection status. Returns #STATUS_OK on success and
 * 			#ERROR_ARGUS_NOT_CONNECTED elsewise.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 ******************************************************************************/
status_t Argus_Ping(argus_hnd_t * hnd);

/*!***************************************************************************
 * @brief 	Evaluate useful information from the raw measurement data.
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
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param 	res : A pointer to the results structure that will be populated
 * 				  with evaluated data.
 * @param 	raw : The pointer to the raw data that has been obtained by the
 * 				  measurement finished callback.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_EvaluateData(argus_hnd_t * hnd, argus_results_t * res,
		ads_value_buf_t * raw);

/****************************************************************************
 * Generic API
 ****************************************************************************/

/*!***************************************************************************
 * @brief	Gets the version number of the current API library.
 * @return 	Returns the current version number.
 *****************************************************************************/
uint32_t Argus_GetAPIVersion(void);

/*!***************************************************************************
 * @brief	Gets the version number of the chip.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @return 	Returns the current version number.
 *****************************************************************************/
uint32_t Argus_GetChipVersion(argus_hnd_t * hnd);

/*!***************************************************************************
 * @brief	Gets the version number of the chip.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @return 	Returns the current version number.
 *****************************************************************************/
uint32_t Argus_GetChipVersion(argus_hnd_t * hnd);

/*!***************************************************************************
 * @brief	Gets the version/variant of the module.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @return 	Returns the current module number.
 *****************************************************************************/
argus_module_version_t Argus_GetModuleVersion(argus_hnd_t * hnd);

/*!***************************************************************************
 * @brief	Gets the unique identification number of the chip.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @return 	Returns the unique identification number.
 *****************************************************************************/
uint32_t Argus_GetChipID(argus_hnd_t * hnd);

/*!***************************************************************************
 * @brief	Gets the SPI hardware slave identifier.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @return	The SPI hardware slave identifier.
 *****************************************************************************/
s2pi_slave_t Argus_GetSPISlave(argus_hnd_t * hnd);

/****************************************************************************
 * Measurement/Device Operation
 ****************************************************************************/

/****************************************************************************
 * Measurement Data API
 ****************************************************************************/

/****************************************************************************
 * Configuration API
 ****************************************************************************/

/*!***************************************************************************
 * @brief 	Sets the measurement mode to a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param 	value : The new measurement mode.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationMeasurementMode(argus_hnd_t * hnd,
		argus_mode_t value);

/*!***************************************************************************
 * @brief 	Gets the measurement mode from a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param 	value : The current measurement mode.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationMeasurementMode(argus_hnd_t * hnd,
		argus_mode_t * value);

/*!***************************************************************************
 * @brief 	Sets the frame time to a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param 	value : The measurement frame time in microseconds.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationFrameTime(argus_hnd_t * hnd, uint32_t value);

/*!***************************************************************************
 * @brief	Gets the frame time from a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param 	value : The current frame time in microseconds.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationFrameTime(argus_hnd_t * hnd, uint32_t * value);

#if 0 // not implemented yet!!!
/*!***************************************************************************
 * @brief 	Sets the target precision to a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param 	precision : The target precision in millimeter.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationTargetPrecision(argus_hnd_t * hnd,
		uint32_t precision);

/*!***************************************************************************
 * @brief	Gets the target precision from a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param 	precision : The current target precision in millimeter.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationTargetPrecision(argus_hnd_t * hnd,
		uint32_t * precision);

/*!***************************************************************************
 * @brief 	Sets the power save mode to a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param 	value : The new power save mode.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationPowerMode(argus_hnd_t * hnd,
		argus_power_mode_t value);

/*!***************************************************************************
 * @brief 	Gets the # from a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param 	value : The current power save mode.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationPowerMode(argus_hnd_t * hnd,
		argus_power_mode_t * value);

/*!***************************************************************************
 * @brief 	Sets the clock mode to a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param 	value : The new clock mode setting.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationClockMode(argus_hnd_t * hnd,
		argus_clock_mode_t value);

/*!***************************************************************************
 * @brief 	Gets the clock mode from a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param 	value : The current clock mode setting.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationClockMode(argus_hnd_t * hnd,
		argus_clock_mode_t * value);

/*!***************************************************************************
 * @brief 	Sets the laser bias current in mA to a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param 	value : The new laser bias current in mA in UQ12.4 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationLaserBiasCurrent(argus_hnd_t * hnd, uq12_4_t value);

/*!***************************************************************************
 * @brief 	Gets the laser bias current in mA from a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param 	value : The current laser bias current in mA in UQ12.4 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationLaserBiasCurrent(argus_hnd_t * hnd, uq12_4_t * value);

/*!***************************************************************************
 * @brief 	Sets the pixel enabled mask to a specified device.
 * @details	The pixel enabled mask determines the ADC channels that are actively
 * 			converting and reading the correlation signals on the chip. Since
 * 			the ADC conversion happens in series, the disabling of pixels in
 * 			hardware results in a time saving it ADC conversion time as well as
 * 			in SPI readout time.
 *
 * 			The pixel enabled mask is an 32-bit mask that determines the
 * 			device internal channel number. It is recommended to use the
 * 			 - #PIXELXY_ISENABLED(msk, x, y)
 * 			 - #PIXELXY_ENABLE(msk, x, y)
 * 			 - #PIXELXY_DISABLE(msk, x, y)
 * 			 .
 * 			macros to work with the pixel enable masks.
 *
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param 	value : The new pixel enabled mask.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationPixelEnabledMask(argus_hnd_t * hnd,
		uint32_t value);

/*!***************************************************************************
 * @brief 	Gets the pixel enabled mask from a specified device.
 * @details	The pixel enabled mask determines the ADC channels that are actively
 * 			converting and reading the correlation signals on the chip. Since
 * 			the ADC conversion happens in series, the disabling of pixels in
 * 			hardware results in a time saving it ADC conversion time as well as
 * 			in SPI readout time.
 *
 * 			The pixel enabled mask is an 32-bit mask that determines the
 * 			device internal channel number. It is recommended to use the
 * 			 - #PIXELXY_ISENABLED(msk, x, y)
 * 			 - #PIXELXY_ENABLE(msk, x, y)
 * 			 - #PIXELXY_DISABLE(msk, x, y)
 * 			 .
 * 			macros to work with the pixel enable masks.
 *
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param 	value : The current pixel enabled mask.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationPixelEnabledMask(argus_hnd_t * hnd,
		uint32_t * value);

/*!***************************************************************************
 * @brief 	Sets the pixel(x,y) enabled value to a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	x : The x-coordinate of the specified pixel.
 * @param	y : The y-coordinate of the specified pixel.
 * @param 	value : The new pixel enabled value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationPixelEnabled(argus_hnd_t * hnd, uint8_t x, uint8_t y, bool value);

/*!***************************************************************************
 * @brief 	Gets the pixel(x,y) enabled value from a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	x : The x-coordinate of the specified pixel.
 * @param	y : The y-coordinate of the specified pixel.
 * @param 	value : The current pixel enabled value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationPixelEnabled(argus_hnd_t * hnd, uint8_t x, uint8_t y, bool * value);

#endif

/*!***************************************************************************
 * @brief 	Sets the full DCA module configuration to a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The new DCA configuration set.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDynamicAdaption(argus_hnd_t * hnd,
		argus_mode_t mode, argus_cfg_dca_t * value);

/*!***************************************************************************
 * @brief 	Gets the # from a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The current DCA configuration set value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDynamicAdaption(argus_hnd_t * hnd,
		argus_mode_t mode, argus_cfg_dca_t * value);

/*!***************************************************************************
 * @brief 	Sets the DCA enabled to a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The new DCA enabled value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDCAEnabled(argus_hnd_t * hnd, argus_mode_t mode,
		argus_dca_enable_t value);

/*!***************************************************************************
 * @brief 	Gets the DCA enabled from a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The current DCA enabled value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDCAEnabled(argus_hnd_t * hnd, argus_mode_t mode,
		argus_dca_enable_t * value);

/*!***************************************************************************
 * @brief 	Sets the DCA saturated pixel threshold count for linear response
 * 			to a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The new DCA linear threshold value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDCALinThreshold(argus_hnd_t * hnd,
		argus_mode_t mode, uint8_t value);

/*!***************************************************************************
 * @brief 	Gets the DCA saturated pixel threshold count for linear response
 * 			from a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The current DCA linear threshold value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDCALinThreshold(argus_hnd_t * hnd,
		argus_mode_t mode, uint8_t * value);

/*!***************************************************************************
 * @brief 	Sets the DCA saturated pixel threshold count for exponential response
 * 			to a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The new DCA exponential threshold value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDCAExpThreshold(argus_hnd_t * hnd,
		argus_mode_t mode, uint8_t value);

/*!***************************************************************************
 * @brief 	Gets the DCA saturated pixel threshold count for exponential response
 * 			from a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The current DCA exponential threshold value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDCAExpThreshold(argus_hnd_t * hnd,
		argus_mode_t mode, uint8_t * value);

/*!***************************************************************************
 * @brief 	Sets the DCA saturated pixel threshold count for reset response
 * 			to a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The new DCA reset threshold value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDCAResetThreshold(argus_hnd_t * hnd,
		argus_mode_t mode, uint8_t value);

/*!***************************************************************************
 * @brief 	Gets the DCA saturated pixel threshold count for reset response
 * 			from a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The current DCA reset threshold value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDCAResetThreshold(argus_hnd_t * hnd,
		argus_mode_t mode, uint8_t * value);

/*!***************************************************************************
 * @brief 	Sets the DCA target amplitude to a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The new DCA target amplitude in UQ12.4 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDCATargetAmplitude(argus_hnd_t * hnd,
		argus_mode_t mode, uq12_4_t value);

/*!***************************************************************************
 * @brief 	Gets the DCA target amplitude from a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The current DCA target amplitude in UQ12.4 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDCATargetAmplitude(argus_hnd_t * hnd,
		argus_mode_t mode, uq12_4_t * value);

/*!***************************************************************************
 * @brief 	Sets the DCA low threshold amplitude to a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The new DCA low threshold amplitude in UQ12.4 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDCALowAmplitude(argus_hnd_t * hnd,
		argus_mode_t mode, uq12_4_t value);

/*!***************************************************************************
 * @brief 	Gets the DCA low threshold amplitude from a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The current DCA low threshold amplitude in UQ12.4 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDCALowAmplitude(argus_hnd_t * hnd,
		argus_mode_t mode, uq12_4_t * value);

/*!***************************************************************************
 * @brief 	Sets the DCA high threshold amplitude to a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The new DCA high threshold amplitude in UQ12.4 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDCAHighAmplitude(argus_hnd_t * hnd,
		argus_mode_t mode, uq12_4_t value);

/*!***************************************************************************
 * @brief 	Gets the DCA high threshold amplitude from a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The current DCA high threshold amplitude in UQ12.4 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDCAHighAmplitude(argus_hnd_t * hnd,
		argus_mode_t mode, uq12_4_t * value);

/*!***************************************************************************
 * @brief 	Sets the DCA average amplitude mode to a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The new DCA average amplitude mode.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDCAAverageAmplitudeMode(argus_hnd_t * hnd,
		argus_mode_t mode, bool value);

/*!***************************************************************************
 * @brief 	Gets the DCA average amplitude mode from a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The current DCA average amplitude mode.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDCAAverageAmplitudeMode(argus_hnd_t * hnd,
		argus_mode_t mode, bool * value);

/*!***************************************************************************
 * @brief 	Sets the DCA nominal integration depth to a specified device.
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
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The new DCA minimum integration depth value in UQ10.6 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDCANomIntegrationDepth(argus_hnd_t * hnd,
		argus_mode_t mode, uq10_6_t value);

/*!***************************************************************************
 * @brief 	Gets the DCA nominal integration depth from a specified device.
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
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The new DCA minimum integration depth value in UQ10.6 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDCANomIntegrationDepth(argus_hnd_t * hnd,
		argus_mode_t mode, uq10_6_t * value);

/*!***************************************************************************
 * @brief 	Sets the DCA minimum integration depth to a specified device.
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
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The new DCA minimum integration depth value in UQ10.6 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDCAMinIntegrationDepth(argus_hnd_t * hnd,
		argus_mode_t mode, uq10_6_t value);

/*!***************************************************************************
 * @brief 	Gets the DCA minimum integration depth from a specified device.
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
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The new DCA minimum integration depth value in UQ10.6 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDCAMinIntegrationDepth(argus_hnd_t * hnd,
		argus_mode_t mode, uq10_6_t * value);

/*!***************************************************************************
 * @brief 	Sets the DCA maximum integration depth to a specified device.
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
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The new DCA maximum integration depth value in UQ10.6 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDCAMaxIntegrationDepth(argus_hnd_t * hnd,
		argus_mode_t mode, uq10_6_t value);

/*!***************************************************************************
 * @brief 	Gets the DCA maximum integration depth from a specified device.
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
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The new DCA maximum integration depth value in UQ10.6 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDCAMaxIntegrationDepth(argus_hnd_t * hnd,
		argus_mode_t mode, uq10_6_t * value);

/*!***************************************************************************
 * @brief 	Sets the DCA nominal/maximal optical power to a specified device.
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
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The new DCA minimum output power in UQ12.4 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDCANomOutputPower(argus_hnd_t * hnd,
		argus_mode_t mode, uq12_4_t value);

/*!***************************************************************************
 * @brief 	Gets the DCA nominal/maximal output power from a specified device.
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
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The current DCA minimum output power in UQ12.4 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDCANomOutputPower(argus_hnd_t * hnd,
		argus_mode_t mode, uq12_4_t * value);

/*!***************************************************************************
 * @brief 	Sets the DCA minimum optical power to a specified device.
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
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The new DCA minimum output power in UQ12.4 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDCAMinOutputPower(argus_hnd_t * hnd,
		argus_mode_t mode, uq12_4_t value);

/*!***************************************************************************
 * @brief 	Gets the DCA minimum output power from a specified device.
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
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The current DCA minimum output power in UQ12.4 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDCAMinOutputPower(argus_hnd_t * hnd,
		argus_mode_t mode, uq12_4_t * value);


/*!***************************************************************************
 * @brief 	Sets the DCA minimal gain stage value to a specified device.
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
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The new DCA minimum gain stage value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDCAMinPixelGain(argus_hnd_t * hnd,
		argus_mode_t mode, argus_dca_gain_t value);

/*!***************************************************************************
 * @brief 	Sets the DCA minimal gain stage value to a specified device.
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
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The new DCA minimum gain stage value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDCAMinPixelGain(argus_hnd_t * hnd,
		argus_mode_t mode, argus_dca_gain_t * value);

/*!***************************************************************************
 * @brief 	Sets the DCA nominal gain stage value to a specified device.
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
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The new DCA nominal gain stage value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDCANomPixelGain(argus_hnd_t * hnd,
		argus_mode_t mode, argus_dca_gain_t value);

/*!***************************************************************************
 * @brief 	Gets the DCA nominal gain stage value from a specified device.
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
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The current DCA nominal gain stage value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDCANomPixelGain(argus_hnd_t * hnd,
		argus_mode_t mode, argus_dca_gain_t * value);

/*!***************************************************************************
 * @brief 	Sets the DCA nominal gain stage value to a specified device.
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
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The new DCA maximum gain stage value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDCAMaxPixelGain(argus_hnd_t * hnd,
		argus_mode_t mode, argus_dca_gain_t value);

/*!***************************************************************************
 * @brief 	Gets the DCA nominal gain stage value from a specified device.
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
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The current DCA maximum gain stage value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDCAMaxPixelGain(argus_hnd_t * hnd,
		argus_mode_t mode, argus_dca_gain_t * value);

/*!***************************************************************************
 * @brief 	Sets the DCA phase shift area to a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The new DCA phase shift area (in %).
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDCAPhaseShiftArea(argus_hnd_t * hnd,
		argus_mode_t mode, uq0_8_t value);

/*!***************************************************************************
 * @brief 	Gets the DCA phase shift area from a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The current DCA phase shift area (in %).
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDCAPhaseShiftArea(argus_hnd_t * hnd,
		argus_mode_t mode, uq0_8_t * value);

/*!***************************************************************************
 * @brief 	Sets the DCA single pulse mode enabled flag to a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The new DCA single pulse mode enabled flag.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationDCASinglePulseMode(argus_hnd_t * hnd,
		argus_mode_t mode,
		bool value);

/*!***************************************************************************
 * @brief 	Gets the DCA single pulse mode enabled flag from a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The current DCA single pulse mode enabled flag.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationDCASinglePulseMode(argus_hnd_t * hnd,
		argus_mode_t mode,
		bool * value);

/*!***************************************************************************
 * @brief 	Sets the pixel binning configuration parameters to a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The new pixel binning configuration parameters.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationPixelBinning(argus_hnd_t * hnd,
		argus_mode_t mode, argus_cfg_pba_t * value);

/*!***************************************************************************
 * @brief 	Gets the pixel binning configuration parameters from a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The current pixel binning configuration parameters.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationPixelBinning(argus_hnd_t * hnd,
		argus_mode_t mode, argus_cfg_pba_t * value);

/*!***************************************************************************
 * @brief 	Sets the # to a specified device.
 * @details	The enabled flags for the pixel binning algorithm, e.g.
 * 			\code PBA_ENABLE | PBA_ENABLE_GOLDPX \endcode to set the global
 * 			enable flag and enable the golden pixel.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The new pixel binning enabled flags.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationPBAEnabled(argus_hnd_t * hnd, argus_mode_t mode,
		argus_cfg_pba_flags_t value);

/*!***************************************************************************
 * @brief 	Gets the # from a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The current pixel binning enabled flags.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationPBAEnabled(argus_hnd_t * hnd, argus_mode_t mode,
		argus_cfg_pba_flags_t * value);

/*!***************************************************************************
 * @brief 	Sets the PBA pre-filter pixel mask to a specified device.
 * @details	The pre-filter pixel mask determines the pixel channels that are
 * 			statically excluded from the pixel binning (i.e. 2D distance) result.
 *
 * 			The pixel enabled mask is an 32-bit mask that determines the
 * 			device internal channel number. It is recommended to use the
 * 			 - #PIXELXY_ISENABLED(msk, x, y)
 * 			 - #PIXELXY_ENABLE(msk, x, y)
 * 			 - #PIXELXY_DISABLE(msk, x, y)
 * 			 .
 * 			macros to work with the pixel enable masks.
 *
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The new pre-filter pixel mask.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationPBAPrefilterPixelMask(argus_hnd_t * hnd,
		argus_mode_t mode, uint32_t value);

/*!***************************************************************************
 * @brief 	Gets the PBA pre-filter pixel mask from a specified device.
 * @details	The pre-filter pixel mask determines the pixel channels that are
 * 			statically excluded from the pixel binning (i.e. 2D distance) result.
 *
 * 			The pixel enabled mask is an 32-bit mask that determines the
 * 			device internal channel number. It is recommended to use the
 * 			 - #PIXELXY_ISENABLED(msk, x, y)
 * 			 - #PIXELXY_ENABLE(msk, x, y)
 * 			 - #PIXELXY_DISABLE(msk, x, y)
 * 			 .
 * 			macros to work with the pixel enable masks.
 *
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The current pre-filter pixel mask.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationPBAPrefilterPixelMask(argus_hnd_t * hnd,
		argus_mode_t mode, uint32_t * value);

/*!***************************************************************************
 * @brief 	Sets the PBA pre-filter pixel(x,y) enabled value to a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param	x : The x-coordinate of the specified pixel.
 * @param	y : The y-coordinate of the specified pixel.
 * @param 	value : The new pre-filter pixel(x,y) enabled value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationPBAPrefilterPixel(argus_hnd_t * hnd,
		argus_mode_t mode, uint8_t x, uint8_t y, bool value);

/*!***************************************************************************
 * @brief 	Gets the PBA pre-filter pixel(x,y) enabled value from a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param	x : The x-coordinate of the specified pixel.
 * @param	y : The y-coordinate of the specified pixel.
 * @param 	value : The current pre-filter pixel(x,y) enabled value.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationPBAPrefilterPixel(argus_hnd_t * hnd,
		argus_mode_t mode, uint8_t x, uint8_t y, bool * value);

/*!***************************************************************************
 * @brief 	Sets the PBA relative amplitude threshold value to a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The new PBA relative amplitude threshold value in UQ0.8 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationPBARelativeThreshold(argus_hnd_t * hnd,
		argus_mode_t mode, uq0_8_t value);

/*!***************************************************************************
 * @brief 	Gets the PBA relative amplitude threshold value from a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The current PBA relative amplitude threshold value in UQ0.8 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationPBARelativeThreshold(argus_hnd_t * hnd,
		argus_mode_t mode, uq0_8_t * value);

/*!***************************************************************************
 * @brief 	Sets the PBA absolute amplitude threshold value to a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The new PBA absolute amplitude threshold value in UQ12.4 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetConfigurationPBAAbsoluteThreshold(argus_hnd_t * hnd,
		argus_mode_t mode, uq12_4_t value);

/*!***************************************************************************
 * @brief 	Gets the PBA absolute amplitude threshold value from a specified device.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	mode : The targeted measurement mode.
 * @param 	value : The current PBA absolute amplitude threshold value in UQ12.4 format.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationPBAAbsoluteThreshold(argus_hnd_t * hnd,
		argus_mode_t mode, uq12_4_t * value);


/*!***************************************************************************
 * @brief 	Gets the current unambiguous range in mm.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param	range_mm : The returned range in mm.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetConfigurationUnambiguousRange(argus_hnd_t * hnd,
		uint32_t * range_mm);

/****************************************************************************
 * Calibration API
 ****************************************************************************/

/*!***************************************************************************
 * @brief	Getter for the current calibration.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param 	cal: Pointer to an existing calibration data structure where the
 * 				 parameters are copied to.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_GetCalibration(argus_hnd_t * hnd, argus_calibration_t * cal);

/*!***************************************************************************
 * @brief	Setter for a new calibration.
 * @details	The new calibration data is applied immediately.
 * @param	hnd : The Argus-API handle; contains all internal states and data.
 * @param 	cal : Pointer to the new calibration data structure that will be
 * 			  	  copied and applied to the device.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
status_t Argus_SetCalibration(argus_hnd_t * hnd, argus_calibration_t * cal);

/*!***************************************************************************
 * @brief 	Getter for an default calibration.
 * @details Populates the #argus_calibration_t structure with some reasonable
 * 			default parameters.
 * @param 	cal : Pointer to an existing calibration data structure where the
 * 				  parameters are copied to.
 * @return	-
 *****************************************************************************/
void Argus_GetDefaultCalibration(argus_calibration_t * cal);

/*! @} */
#endif /* ARGUS_API_H */
