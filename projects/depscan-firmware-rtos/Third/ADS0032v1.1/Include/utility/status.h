/*************************************************************************//**
 * @file
 * @brief    	This file is part of the AFBR-S50 API.
 * @details		This file contains status codes for all platform specific
 * 				functions.
 * 
 * @copyright	Copyright c 2016-2019, Avago Technologies GmbH.
 * 				All rights reserved.
 *****************************************************************************/

#ifndef STATUS_H
#define STATUS_H

/*!***************************************************************************
 * @defgroup	status Status Codes
 * @brief		Status and error code definitions
 * @details		Defines status and error codes for function return values.
 * @addtogroup 	status
 * @{
 *****************************************************************************/

#include <stdint.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! Generic status and error return codes.
 *  @ingroup status */
enum StatusGeneric
{
	/*! Status for success/no error. */
	STATUS_OK = 0,

	/*! Status for device/module/hardware idle. Implies #STATUS_OK. */
	STATUS_IDLE = 0,

	/*! Status for device/module/hardware busy. */
	STATUS_BUSY = 2,

	/*! Status for device/module/hardware is currently initializing. */
	STATUS_INITIALIZING = 3,

	/*! Error for generic fail/error. */
	ERROR_FAIL = -1,

	/*! Error for process aborted by user/external. */
	ERROR_ABORTED = -2,

	/*! Error for invalid read only operations. */
	ERROR_READ_ONLY = -3,

	/*! Error for out of range parameters. */
	ERROR_OUT_OF_RANGE = -4,

	/*! Error for invalid argument passed to an function. */
	ERROR_INVALID_ARGUMENT = -5,

	/*! Error for timeout occurred. */
	ERROR_TIMEOUT = -6,

	/*! Error for not initialized modules. */
	ERROR_NOT_INITIALIZED = -7,

	/*! Error for not supported. */
	ERROR_NOT_SUPPORTED = -8,

	/*! Error for yet not implemented functions. */
	ERROR_NOT_IMPLEMENTED = -9,
};

/*!***************************************************************************
 * @brief	Type used for all status and error return values.
 * @details - 0 is OK or no error.
 * 			- negative values determine errors.
 * 			- positive values determine warnings or status information.
 *****************************************************************************/
typedef int32_t status_t;

/*! @} */
#endif /* STATUS_H */
