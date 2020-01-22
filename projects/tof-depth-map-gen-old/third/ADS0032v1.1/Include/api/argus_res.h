/*************************************************************************//**
 * @file
 * @brief    	This file is part of the Argus hardware API.
 * @details		Defines the generic Argus API results data structure.
 * 
 * @copyright	Copyright c 2016-2018, Avago Technologies GmbH.
 * 				All rights reserved.
 *****************************************************************************/

#ifndef ARGUS_RES_H
#define ARGUS_RES_H

/*!***************************************************************************
 * @defgroup	argusres Measurement Data API
 * @ingroup		argusapi
 * @brief		Argus measurement results data structure.
 * @details		Structure that gathers all the results from the Argus device.
 * 				E.g. distance and amplitude values.
 * @addtogroup 	argusres
 * @{
 *****************************************************************************/

#include "argus_def.h"
#include "argus_px.h"
#include "argus_meas.h"

/*!***************************************************************************
 * @brief	The 1d measurement results data structure.
 * @details The 1d measurement results obtained by the pixel binning algorithm.
 *****************************************************************************/
typedef struct Argus_1dResults
{
	/*! Mask of pixels that are used to obtain the 1D measurement results. */
	uint32_t PixelMask;

	/*! Raw 1D range value in meter (Q9.22 format). The distance obtained by
	 *  the pixel binning algorithm from the current measurement frame. */
	q9_22_t Range;

	/*! The 1D amplitude in LSB (Q12.4 format). The (maximum) amplitude obtained
	 *  by the pixel binning algorithm from the current measurement frame.
	 *  Special value: 0 == No/Invalid Result. */
	uq12_4_t Amplitude;

} argus_results_bin_t;

/*!***************************************************************************
 * @brief	The auxiliary measurement results data structure.
 * @details The auxiliary measurement results obtained by the auxiliary task.
 * 			Special values, i.e. 0xFFFFU, indicate no readout value available.
 *****************************************************************************/
typedef struct Argus_AuxResults
{
	/*! VDD ADC channel readout value.
	 *  Special Value if no value has been measured:
	 *  Invalid/NotAvailable = 0xFFFFU (UQ12_4_MAX) */
	uq12_4_t VDD;

	/*! Temperature sensor ADC channel readout value.
	 *  Special Value if no value has been measured:
	 *  Invalid/NotAvailable = 0x7FFFU (Q11_4_MAX) */
	q11_4_t TEMP;

	/*! Substrate Voltage ADC Channel readout value.
	 *  Special Value if no value has been measured:
	 *  Invalid/NotAvailable = 0xFFFFU (UQ12_4_MAX) */
	uq12_4_t VSUB;

	/*! VDD VCSEL ADC channel readout value.
	 *  Special Value if no value has been measured:
	 *  Invalid/NotAvailable = 0xFFFFU (UQ12_4_MAX) */
	uq12_4_t VDDL;

	/*! APD current ADC Channel readout value.
	 *  Special Value if no value has been measured:
	 *  Invalid/NotAvailable = 0xFFFFU (UQ12_4_MAX) */
	uq12_4_t IAPD;

} argus_results_aux_t;

/*!***************************************************************************
 * @brief	The measurement results data structure.
 * @details Measurement data from the device.
 * @code
 * 			// Pixel Field: Pixel[x][y]
 * 			//
 * 			// y  O O O O O O O O
 * 			// ^   O O O O O O O O
 * 			// |  O O O O O O O O          O (ref. Px)
 * 			// |   O O O O O O O O
 * 			// 0  -----------> x
 * @endcode
 *****************************************************************************/
typedef struct Argus_Results
{
	/*! The \link #status_t status\endlink of the current measurement frame.
	 *   - 0 (i.e. #STATUS_OK) for a good measurement signal.
	 *   - > 0 for warnings and weak measurement signal.
	 *   - < 0 for errors and invalid measurement signal. */
	status_t Status;

	/*! Time in milliseconds (measured since the last MCU startup/reset)
	 *  when the measurement was triggered. */
	ltc_t TimeStamp;

	/*! ADC channel saturated mask for the first
	 *  channels 0 .. 31 (active pixels channels). */
	uint32_t PxSatMask;

	/*! ADS channel saturated mask for the remaining
	 *  channels 31 .. 63 (miscellaneous values). */
	uint32_t ChSatMask;

	/*! The configuration for the current measurement frame. */
	argus_meas_frame_t Frame;

	/*! Raw unmapped ADC results from the device. */
	uint8_t Data[ARGUS_RAW_DATA_SIZE];

	/*! Raw Range Values from the device in meter.
	 *  It is the actual distance before software adjustments/calibrations. */
	argus_pixel_t PixelRef;

	/*! Raw Range Values from the device in meter.
	 *  It is the actual distance before software adjustments/calibrations. */
	argus_pixel_t Pixel[ARGUS_PIXELS_X][ARGUS_PIXELS_Y];

	/*! Pixel binned results. */
	argus_results_bin_t Bin;

	/*! The auxiliary ADC channel data. */
	argus_results_aux_t Auxiliary;

} argus_results_t;

/*! @} */
#endif /* ARGUS_RES_H */
