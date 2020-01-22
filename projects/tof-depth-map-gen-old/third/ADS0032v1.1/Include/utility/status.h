/*************************************************************************//**
 * @file
 * @brief    	This file is part of the Argus hardware API.
 * @details		This file contains status codes for all platform specific
 * 				functions.
 * 
 * @copyright	Copyright c 2016-2018, Avago Technologies GmbH.
 * 				All rights reserved.
 *****************************************************************************/

#ifndef STATUS_H
#define STATUS_H

/*!***************************************************************************
 * @defgroup	status Status Codes
 * @brief		Status Codes Definitions
 * @details		Defines status codes for specific functions.
 * @addtogroup 	status
 * @{
 *****************************************************************************/

#include <stdint.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! Generic status and error return codes. */
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

/*! Type used for all status and error return values. */
typedef int32_t status_t;

/*! @} */
#endif /* STATUS_H */
