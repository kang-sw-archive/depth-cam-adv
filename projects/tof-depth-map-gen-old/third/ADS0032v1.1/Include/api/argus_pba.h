/*************************************************************************//**
 * @file
 * @brief    	This file is part of the Argus hardware API.
 * @details		Defines the generic Argus API calibration data structure.
 * 
 * @copyright	Copyright c 2016-2018, Avago Technologies GmbH.
 * 				All rights reserved.
 *****************************************************************************/

#ifndef ARGUS_CFG_PBA_H
#define ARGUS_CFG_PBA_H

/*!***************************************************************************
 * @defgroup 	arguspba Pixel Binning Algorithm (PBA) API
 * @ingroup		arguscfg
 * @brief 		This module manages the pixel binning algorithm.
 * @details		The PBA module contains filter algorithms that determine the
 * 				pixels with the best signal quality and extract an 1-d distance
 * 				information from the filtered pixels.
 *
 * @addtogroup 	arguspba
 * @{
 *****************************************************************************/

#include "argus_def.h"

/*! The minimum golden pixel x-index. */
#define ARGUS_CFG_PBA_GOLDEN_PX_X_MIN 	(0)

/*! The maximum golden pixel x-index. */
#define ARGUS_CFG_PBA_GOLDEN_PX_X_MAX 	(ARGUS_PIXELS_X-1)

/*! The minimum golden pixel y-index. */
#define ARGUS_CFG_PBA_GOLDEN_PX_Y_MIN 	(0)

/*! The maximum golden pixel y-index. */
#define ARGUS_CFG_PBA_GOLDEN_PX_Y_MAX 	(ARGUS_PIXELS_Y-1)

/*!***************************************************************************
 * @brief	Enable flags for the pixel binning algorithm.
 *****************************************************************************/
typedef enum Argus_CFG_PBA_Flags
{
	/*! Enable pixel binning. */
	PBA_ENABLE = 0x01U,

//	/*! Enable pixel binning pre-filter. */
//	PBA_ENABLE_PREFILTER = 0x02U,

//	/*! Enable pixel binning absolute threshold value. */
//	PBA_ENABLE_ABSTH = 0x04U,

//	/*! Enable pixel binning relative threshold value. */
//	PBA_ENABLE_RELTH = 0x08U,

//	/*! Enable pixel binning average threshold value. */
//	PBA_ENABLE_AVGTH = 0x10U,

	/*! Enable pixel binning golden pixel. */
	PBA_ENABLE_GOLDPX = 0x20U,

//	/*! Enable pixel binning maximum threshold value. */
//	PBA_ENABLE_MAXTH = 0x40U,

} argus_cfg_pba_flags_t;

/*!***************************************************************************
 * @brief	The operation modes for the pixel binning algorithm.
 *****************************************************************************/
typedef enum Argus_CFG_PBA_Mode
{
	/*! Evaluate the average range from all available pixels. */
	PBA_MODE_AVG = 1U,

	/*! Evaluate the minimum range from all available pixels. */
	PBA_MODE_MIN = 2U,

} argus_cfg_pba_mode_t;

/*!***************************************************************************
 * @brief	The Argus pixel binning algorithm settings data structure.
 * @details	Describes the pixel binning algorithm settings.
 *****************************************************************************/
typedef struct Argus_CFG_PBA
{
	/*! Enables the pixel binning features.
	 *  Each bit may enable a different feature. See #argus_cfg_pba_flags_t
	 *  for details about the enabled flags. */
	argus_cfg_pba_flags_t Enabled;

	/*! Determines the PBA evaluation mode which is used to obtain the
	 *  final range value from the algorithm, for example, the average
	 *  of all pixels. See #argus_cfg_pba_mode_t for more details about
	 *  the individual evaluation modes. */
	argus_cfg_pba_mode_t Mode;

	/*! The pre-filter pixel mask determines the pixel channels that are
	 * 	statically excluded from the pixel binning (i.e. 1D distance) result.
	 *
	 * 	The pixel enabled mask is an 32-bit mask that determines the
	 * 	device internal channel number. It is recommended to use the
	 * 	 - #PIXELXY_ISENABLED(msk, x, y)
	 * 	 - #PIXELXY_ENABLE(msk, x, y)
	 * 	 - #PIXELXY_DISABLE(msk, x, y)
	 * 	 .
	 * 	macros to work with the pixel enable masks. */
	uint32_t PrefilterMask;

	/*! The Relative amplitude threshold value (in %) of the max. amplitude.
	 *  Pixels with amplitude below this threshold value are dismissed.
	 *
	 *  All available values from the 8-bit representation are valid.
	 *  The actual percentage value is determined by 100%/256*x.
	 *
	 *  Use 0 to disable the relative amplitude threshold. */
	uq0_8_t RelAmplThreshold;

	/*! The Absolute amplitude threshold value in LSB.
	 * 	Pixels with amplitude below this threshold value are dismissed.
	 *
	 *  All available values from the 16-bit representation are valid.
	 *  The actual LSB value is determined by x/16.
	 *
	 *  Use 0 to disable the absolute amplitude threshold. */
	uq12_4_t AbsAmplThreshold;

} argus_cfg_pba_t;

/*! @} */
#endif /* ARGUS_CFG_PBA_H */
