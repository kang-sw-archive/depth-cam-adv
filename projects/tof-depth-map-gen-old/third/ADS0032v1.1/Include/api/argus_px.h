/*************************************************************************//**
 * @file
 * @brief    	This file is part of the Argus hardware API.
 * @details		Defines the generic Argus API results data structure.
 * 
 * @copyright	Copyright c 2016-2018, Avago Technologies GmbH.
 * 				All rights reserved.
 *****************************************************************************/

#ifndef ARGUS_PX_H
#define ARGUS_PX_H

/*!***************************************************************************
 * @addtogroup 	argusres
 * @{
 *****************************************************************************/

/*! Maximum amplitude value in UQ12.4 format. */
#define ARGUS_AMPLITUDE_MAX		(0xFFF0U)

/*!***************************************************************************
 * @brief	Status flags for the evaluated pixel structure.
 *****************************************************************************/
typedef enum Argus_Pixel_Status
{
	/*! 0x00: Pixel status OK. */
	PIXEL_OK = 0,

	/*! 0x01: Pixel is disabled. */
	PIXEL_OFF = 1U << 0U,

	/*! 0x02: Pixel is saturated (i.e. at least one saturation bit for any
	 *        sample is set or the sample is in the invalidity area). */
	PIXEL_SAT = 1U << 1U,

	/*! 0x04: Pixel is excluded from the pixel binning result. */
	PIXEL_BIN_EXCL = 1U << 2U,

	/*! 0x08: Pixel amplitude minimum underrun (i.e. the amplitude calculated to 0).  */
	PIXEL_AMPL_MIN = 1U << 3U,

	/*! 0x10: Pixel amplitude maximum overrun (i.e. the amplitude calculated >= 4096).  */
	PIXEL_AMPL_MAX = 1U << 4U,

} argus_px_status_t;

/*!***************************************************************************
 * @brief	The evaluated measurement results per pixel.
 * @details	This structure contains the evaluated data for a single pixel.\n
 * 			If the amplitude is 0, the pixel is turned off or has invalid data.
 *****************************************************************************/
typedef struct Argus_Pixel
{
	/*! Range Values from the device in meter. It is the actual distance before
	 *  software adjustments/calibrations. */
	q9_22_t Range;

	/*! Phase Values from the device in units of PI, i.e. 0 ... 2. */
	uq1_15_t Phase;

	/*! Amplitudes of measured signals in LSB.
	 *  Special values: 0 == Pixel Off, 0xFFFF == Overflow/Error */
	uq12_4_t Amplitude;

	/*! Pixel status; determines if the pixel is disabled, overflown or could
	 *  not be evaluated correctly. */
	argus_px_status_t Status;

} argus_pixel_t;

/*! @} */
#endif /* ARGUS_PX_H */
