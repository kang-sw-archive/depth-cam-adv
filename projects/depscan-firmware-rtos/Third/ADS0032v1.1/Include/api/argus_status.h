/*************************************************************************//**
 * @file
 * @brief    	This file is part of the AFBR-S50 API.
 * @details		Provides status codes for the AFBR-S50 API.
 * 
 * @copyright	Copyright c 2016-2019, Avago Technologies GmbH.
 * 				All rights reserved.
 *****************************************************************************/

#ifndef ARGUS_STATUS_H
#define ARGUS_STATUS_H

/*!***************************************************************************
 * @addtogroup 	status
 * @{
 *****************************************************************************/

#include "utility/status.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! AFBR-S50 API status and error return codes.
 *  @ingroup status */
enum StatusArgus
{
	/*! AFBR-S50 Status: Internal status when the device module is initializing
	 *  a new measurement. */
	STATUS_ARGUS_STARTING = 100,

	/*! AFBR-S50 Status: Device active with measurements. */
	STATUS_ARGUS_ACTIVE = 103,

	/*! AFBR-S50 Status: Measurement was not executed due to lack of available
	 *  data buffers. Please call #Argus_EvaluateData to free the buffers. */
	STATUS_ARGUS_BUFFER_BUSY = 104,

	/*! AFBR-S50 Status: Measurement was not executed due to output power
	 *  limitations. */
	STATUS_ARGUS_POWERLIMIT = 105,

	/*! AFBR-S50 Status: DCA has determined large signals and can not adjust the
	 * integration energy any further.
	 * The current results should be considered carefully. */
	STATUS_ARGUS_OVERFLOW = 106,

	/*! AFBR-S50 Status: DCA has determined small signals and can not adjust
	 * integration energy any further.
	 * The current results should be considered carefully. */
	STATUS_ARGUS_UNDERFLOW = 107,

	/*! AFBR-S50 Status: The readout algorithm for the EEPROM has detected a bit
	 *  error which has been corrected. However, if more than a single bit error
	 *  has occurred, the corrected value is invalid! This cannot be distinguished
	 *  from the valid case. Thus, if the error starts to occur, the sensor
	 *  should be replaced soon! */
	STATUS_ARGUS_EEPROM_BIT_ERROR = 109,

	/*! AFBR-S50 Status: Inconsistent EEPROM readout data. No calibration
	 *  trimming values are applied. The calibration remains invalid. */
	STATUS_ARGUS_INVALID_EEPROM = 110,

	/*! AFBR-S50 Error: No device connected. Initial SPI tests failed. */
	ERROR_ARGUS_NOT_CONNECTED = -101,

	/*! AFBR-S50 Error: Inconsistent configuration parameters. */
	ERROR_ARGUS_INVALID_CFG = -102,

	/*! AFBR-S50 Error: Either all pixels are saturated in pixel binning
	 *  algorithm or DCA has determined large number of saturated pixels and is
	 *  trying to adjust the integration energy correspondingly. Current
	 *  measurement frame result is invalid. */
	ERROR_ARGUS_SATURATED = -103,

	/*! AFBR-S50 Error: Invalid measurement mode configuration parameter. */
	ERROR_ARGUS_INVALID_MODE = -105,

	/*! AFBR-S50 Error: The APD bias voltage is reinitializing due to a dropout.
	 *  The current measurement data set is invalid! */
	ERROR_ARGUS_BIAS_VOLTAGE_REINIT = -107,

	/*! AFBR-S50 Error: The signal is out of sync with respect to the dual
	 *  frequency mode, i.e. the noise is too large to estimate the unambiguous
	 *  window. */
	ERROR_ARGUS_SIGNAL_OUT_OF_SYNC = -108,

	/*! AFBR-S50 Error: The EEPROM readout has failed. The failure is detected
	 *  by three distinct read attempts, each resulting in invalid data.
	 *  Note: this state differs from that #STATUS_ARGUS_EEPROM_BIT_ERROR
	 *  such that it is usually temporarily and due to harsh ambient conditions. */
	ERROR_ARGUS_EEPROM_FAILURE = -109,

	/*! AFBR-S50 Error: The measurement signal is invalid and thus the result
	 *  value (in 1D range) is stalled. This means the value is invalid and kept
	 *  at the previous valid value. */
	ERROR_ARGUS_STALLED = -110,

	/*! AFBR-S50 Error: The background light is too bright. */
	ERROR_ARGUS_BGL_EXCEEDANCE = -111,

	/*! AFBR-S50 Error: The crosstalk vector amplitude is too high. */
	ERROR_ARGUS_XTALK_AMPLITUDE_EXCEEDANCE = -112,

	/*! AFBR-S50 Error: Laser malfunction! Laser Safety may not be given! */
	ERROR_ARGUS_LASER_FAILURE = -113,
};

/*! @} */
#endif /* ARGUS_STATUS_H */
