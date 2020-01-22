/*************************************************************************//**
 * @file
 * @brief    	This file is part of the Argus hardware API.
 * @details		Provides status codes for the Argus API.
 * 
 * @copyright	Copyright c 2016-2018, Avago Technologies GmbH.
 * 				All rights reserved.
 *****************************************************************************/

#ifndef ARGUS_STATUS_H
#define ARGUS_STATUS_H

/*!***************************************************************************
 * @defgroup	status Status Codes
 * @brief		Status Codes Definitions
 * @details		Defines status codes for specific functions.
 * @addtogroup 	status
 * @{
 *****************************************************************************/

#include "utility/status.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! Argus API status and error return codes. */
enum StatusArgus
{
	/*! Argus API status: Internal status when the device module is initializing
	 *  a new measurement. */
	STATUS_ARGUS_STARTING = 100,

//	/*! Argus API status: currently performing a single shot measurement. */
//	STATUS_ARGUS_SINGLE_MEASUREMENT = 101,

//	/*! Argus API status: currently performing iterative timer triggered measurements. */
//	STATUS_ARGUS_ITERATIVE_MEASUREMENTS = 102,

	/*! Argus API status: device busy with integration. */
	STATUS_ARGUS_RUNNING = 103,

	/*! Argus API status: measurement was not executed due to lack of available
	 *  data buffer. */
	STATUS_ARGUS_BUFFER_BUSY = 104,

	/*! Argus API status: measurement was not executed due to output power
	 *  limitations. */
	STATUS_ARGUS_POWERLIMIT = 105,

	/*! Argus API status: DCA has determined large signals and will adjust the
	 * integration depth correspondingly.
	 * The current results should be considered carefully.*/
	STATUS_ARGUS_OVERFLOW = 106,

	/*! Argus API status: DCA has determined small signals and will adjusting
	 * integration depth correspondingly.
	 * The current results should be considered carefully. */
	STATUS_ARGUS_UNDERFLOW = 107,

	/*! Argus API status: DCA has determined the device to operate at the
	 *  bandwidth limit and is adjusting the global phase shift
	 *  correspondingly. The current results should be considered carefully. */
	STATUS_ARGUS_BANDWITH_LIMIT = 108,

	/*! Argus API status: The readout algorithm for the EEPROM has detected
	 *  and bit error which has been corrected. However, if more than a single
	 *  bit error occurred, the corrected value is invalid! This cannot be
	 *  distinguished from the valid case. */
	STATUS_ARGUS_EEPROM_BIT_ERROR = 109,

	/*! Inconsistent EEPROM data. No trimming values are applied. */
	STATUS_ARGUS_INVALID_EEPROM = 110,

	/*! No device connected. Initial SPI tests failed. */
	ERROR_ARGUS_NOT_CONNECTED = -101,

	/*! Inconsistent configuration parameters. */
	ERROR_ARGUS_INVALID_CFG = -102,

	/*! Argus API status: Either all pixels are saturated in pixel binning
	 *  algorithm or DCA has determined large number of saturated pixels and is
	 *  adjusting the integration energy correspondingly. Current results should
	 *  be considered invalid. */
	ERROR_ARGUS_SATURATED = -103,

	/*! Argus API status: DCA has entered some race condition. Low and high
	 *  conditions are competing and the algorithm fails! */
	ERROR_ARGUS_DCA_RACE_CONDITION = -104,

	/*! Invalid measurement mode configuration parameter. */
	ERROR_ARGUS_INVALID_MODE = -105,

	/*! The APD bias voltage is reinitializing due to a dropout.
	 *  The current measurement data set is invalid! */
	ERROR_ARGUS_BIAS_VOLTAGE_REINIT = -107,

};

/*! @} */
#endif /* ARGUS_STATUS_H */
