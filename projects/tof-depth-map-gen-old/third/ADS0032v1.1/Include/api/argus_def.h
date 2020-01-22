/*************************************************************************//**
 * @file
 * @brief    	This file is part of the Argus hardware API.
 * @details		This file provides generic definitions belonging to all
 * 				devices from the Argus product family.
 *
 * @copyright	Copyright (c) 2016-2018, Avago Technologies GmbH.
 * 				All rights reserved.
 *****************************************************************************/

#ifndef ARGUS_DEF_H
#define ARGUS_DEF_H

/*!***************************************************************************
 * Include files
 *****************************************************************************/
#include "argus_status.h"
#include "argus_version.h"
#include "utility/fp_def.h"
#include "utility/time.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*!***************************************************************************
 * @addtogroup 	argusapi
 * @{
 *****************************************************************************/

/*! Argus Chip v1.0 */
#define ARGUS_CHIP_NONE 0

/*! Argus Chip v1.0 */
#define ARGUS_CHIP_V1_0 MAKE_VERSION(1, 0, 0)

/*! Argus Chip v1.1 */
#define ARGUS_CHIP_V1_1 MAKE_VERSION(1, 1, 0)


/*!***************************************************************************
 * @brief	The Argus Chip Version.
 * @details Available Versions:
 * 			- v1.0: ARGUS_CHIP_V1_0
 * 			- v1.1: ARGUS_CHIP_V1_1
 * 			.
 *****************************************************************************/
#define ARGUS_CHIP_VERSION ARGUS_CHIP_V1_1

/*!***************************************************************************
 * @brief	The Argus Chip Name.
 *****************************************************************************/
#if ARGUS_CHIP_VERSION == ARGUS_CHIP_V1_0
#define ARGUS_CHIP_NAME "ADS0032 v1.0"
#elif ARGUS_CHIP_VERSION == ARGUS_CHIP_V1_1
#define ARGUS_CHIP_NAME "ADS0032 v1.1"
#else
#define ARGUS_CHIP_NAME "ADS0032"
#endif

/*!***************************************************************************
 * @brief	Data logging via serial connection.
 * @warning	Logging decreases the measurement rate drastically!
 *****************************************************************************/
#define ARGUS_LOGGING 0

/*!***************************************************************************
 * @brief	Hardware (SPI) trace via serial connection.
 * @warning	Tracing decreases the measurement rate drastically!
 *****************************************************************************/
#define ARGUS_TRACE 0

/*!***************************************************************************
 * @brief	Error logging via serial connection.
 * @warning	Logging decreases the measurement rate drastically!
 *****************************************************************************/
#define ARGUS_ERROR_LOGGING 1

/*!***************************************************************************
 * @brief	Maximum number of phases per measurement cycle.
 * @details	The actual phase number is defined in the register configuration.
 * 			However the software does only support a fixed value of 4 yet.
 *****************************************************************************/
#define ARGUS_PHASECOUNT 4U

/*!***************************************************************************
 * @brief	The Argus pixel field size in x direction (long edge).
 *****************************************************************************/
#define ARGUS_PIXELS_X	8U

/*!***************************************************************************
 * @brief	The Argus pixel field size in y direction (short edge).
 *****************************************************************************/
#define ARGUS_PIXELS_Y	4U

/*!***************************************************************************
 * @brief	The Argus pixel count.
 *****************************************************************************/
#define ARGUS_PIXELS	((ARGUS_PIXELS_X)*(ARGUS_PIXELS_Y))


/*!***************************************************************************
 * @brief	The Argus module types.
 *****************************************************************************/
typedef enum ArgusModuleVersion
{
	/*! No device connected or not recognized. */
	MODULE_NONE = 0,

	/*! AFBR-S50MV85G: an ADS0032 based multi-pixel range finder device
	 *  w/ 4x8 pixel matrix. */
	AFBR_S50MV85G_V1 = 1,

} argus_module_version_t;

/*!***************************************************************************
 * @brief	The Argus chip versions.
 *****************************************************************************/
typedef enum ArgusChipVersion
{
	/*! No device connected or not recognized. */
	ADS0032_NONE = ARGUS_CHIP_NONE,

	/*! ADS0032 v1.0 */
	ADS0032_V1_0 = ARGUS_CHIP_V1_0,

	/*! ADS0032 v1.1 */
	ADS0032_V1_1 = ARGUS_CHIP_V1_1

} argus_chip_version_t;

/*!***************************************************************************
 * @brief	The number of measurement modes with distinct configuration and
 * 			calibration records.
 *****************************************************************************/
#define ARGUS_MODE_COUNT (2)


/*!***************************************************************************
 * @brief	The Argus measurement mode.
 *****************************************************************************/
typedef enum Argus_Mode
{
	/*! Measurement Mode A: Baseline Mode = Long Range Mode. */
	ARGUS_MODE_A = 1,

	/*! Measurement Mode B: Precision Mode = Short Range Mode. */
	ARGUS_MODE_B = 2,

} argus_mode_t;


/*!***************************************************************************
 * @brief	The clock mode.
 *****************************************************************************/
typedef enum
{
	/*! Intern clock mode: internal RC oscillator is used for clock generation. */
	ARGUS_CLOCK_MODE_INTERN,

	/*! Extern clock mode: external source is used for clock generation.
	 *  (Not supported yet!!) */
	ARGUS_CLOCK_MODE_EXTERN
} argus_clock_mode_t;


/*!***************************************************************************
 * @brief	Generic argus callback function.
 * @details	Invoked by the API. The content of the abstract data pointer
 * 			depends upon the context.
 * @param	status : The module status that caused the callback. #STATUS_OK if
 * 					 everything was as expected.
 * @param	data   : An abstract pointer to an user defined data. This will
 * 					 usually be passed to the function that also takes the
 * 					 callback as an parameter. Otherwise it has a special
 * 					 meaning such as configuration or calibration data.
 * @return 	Returns the \link #status_t status\endlink (#STATUS_OK on success).
 *****************************************************************************/
typedef status_t (*argus_callback_t)(status_t status, void * data);

/*! @} */
#endif /* ARGUS_DEF_H */
