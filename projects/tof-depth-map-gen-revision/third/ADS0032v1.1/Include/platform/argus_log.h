/*************************************************************************//**
 * @file
 * @brief    	This file is part of the Argus API.
 * @details		This file provides an interface for the required debug module.
 * 
 * @copyright	Copyright c 2016-2018, Avago Technologies GmbH.
 * 				All rights reserved.
 *****************************************************************************/

#ifndef ARGUS_LOG_H
#define ARGUS_LOG_H

/*!***************************************************************************
 * @defgroup	argus_log Debug: Logging Interface
 * @ingroup		argus_platform
 * @brief		Logging interface for the Argus API.
 * @details		This interface provides logging utility functions.
 * @addtogroup 	argus_log
 * @{
 *****************************************************************************/

#include "api/argus_def.h"

/*!***************************************************************************
 * @brief	A print() function; externally defined.
 * @param 	fmt_s : The usual print() format string.
 * @param 	... : The usual print() parameters.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 * @note	The naming is different from the standard printf() on purpose to
 * 			prevent builtin compiler optimizations.
 *****************************************************************************/
extern status_t print(const char *fmt_s, ...);

/*******************************************************************************
 * Logging Functions
 ******************************************************************************/

#if ARGUS_TRACE
/*!***************************************************************************
 * @brief	General hardware trace function.
 *
 * @details	Redirected to print()
 *
 * @param 	fmt : format string (prinft style)
 * @param	... : parameters
 * @return	-
 *****************************************************************************/
#define trace(fmt, ...) print("TRACE: " fmt "\n", ##__VA_ARGS__)
#else
/*!***************************************************************************
 * @brief	General hardware trace function (disabled by precompiler flag).
 *
 * @details	Redirected to print()
 *
 * @param	... : parameters
 * @return	-
 *****************************************************************************/
#define trace(...) (void)0
#endif

#if ARGUS_LOGGING
/*!***************************************************************************
 * @brief	General logging function.
 *
 * @details	Redirected to print()
 *
 * @param 	fmt : format string (prinft style)
 * @param	... : parameters
 * @return	-
 *****************************************************************************/
#define log(fmt, ...) print("LOG: " fmt "\n", ##__VA_ARGS__)
#else
/*!***************************************************************************
 * @brief	General logging function (disabled by precompiler flag).
 *
 * @details	Redirected to print()
 *
 * @param	... : parameters
 * @return	-
 *****************************************************************************/
#define log(...) (void)0
#endif

#if ARGUS_ERROR_LOGGING
/*!***************************************************************************
 * @brief	Error logging function.
 *
 * @details	Redirected to print()
 *
 * @param 	fmt : format string (prinft style)
 * @param	... : parameters
 * @return	-
 *****************************************************************************/
#define error_log(fmt, ...) print("ERROR: " fmt "\n", ##__VA_ARGS__)
#else
/*!***************************************************************************
 * @brief	Error logging function (disabled by precompiler flag).
 *
 * @details	Redirected to print()
 *
 * @param	... : parameters
 * @return	-
 *****************************************************************************/
#define error_log(...) (void)0
#endif

/*! @} */
#endif /* ARGUS_DEBUG_H */
