/*************************************************************************//**
 * @file
 * @brief    	This file is part of the AFBR-S50 API.
 * @details		Defines the device pixel measurement results data structure.
 * 
 * @copyright	Copyright c 2016-2019, Avago Technologies GmbH.
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
 *
 * @details Determines the pixel status. 0 means OK (#PIXEL_OK).
 * 			- [0]: #PIXEL_OFF: Pixel was disabled and not read from the device.
 * 			- [1]: #PIXEL_SAT: The pixel was saturated.
 * 			- [2]: #PIXEL_BIN_EXCL: The pixel was excluded from the 1D result.
 * 			- [3]: #PIXEL_AMPL_MIN: The pixel amplitude has evaluated to 0.
 * 			- [4]: #PIXEL_AMPL_MAX: The pixel amplitude has evaluated to max.
 * 			- [5]: #PIXEL_NO_SIGNAL: The pixel has no valid signal.
 * 			- [6]: #PIXEL_OUT_OF_SYNC: The pixel has lost signal trace.
 * 			- [7]: #PIXEL_STALLED: The pixel value is stalled due to errors.
 * 			.
 *****************************************************************************/
typedef enum
{
	/*! 0x00: Pixel status OK. */
	PIXEL_OK = 0,

	/*! 0x01: Pixel is disabled and not data has been read from the device. */
	PIXEL_OFF = 1U << 0U,

	/*! 0x02: Pixel is saturated (i.e. at least one saturation bit for any
	 *        sample is set or the sample is in the invalidity area). */
	PIXEL_SAT = 1U << 1U,

	/*! 0x04: Pixel is excluded from the pixel binning (1d) result. */
	PIXEL_BIN_EXCL = 1U << 2U,

	/*! 0x08: Pixel amplitude minimum underrun
	 *        (i.e. the amplitude calculation yields 0). */
	PIXEL_AMPL_MIN = 1U << 3U,

	/*! 0x10: Pixel amplitude maximum overrun
	 *        (i.e. the amplitude calculated yields >= 4096). */
	PIXEL_AMPL_MAX = 1U << 4U,

	/*! 0x20: Pixel amplitude is below its threshold value. The received signal
	 *  	  strength is too low to evaluate a valid signal. The range value is
	 *  	  set to the maximum possible value (approx. 512 m). */
	PIXEL_NO_SIGNAL = 1U << 5U,

	/*! 0x40: Pixel is not in sync with respect to the dual frequency algorithm.
	 * 		  I.e. the pixel may have a correct value but is estimated into the
	 * 		  wrong unambiguous window. */
	PIXEL_OUT_OF_SYNC = 1U << 6U,

	/*! 0x80: Pixel is stalled due to one of the following reasons:
	 * 		  - #PIXEL_SAT
	 * 		  - #PIXEL_AMPL_MIN
	 * 		  - #PIXEL_AMPL_MAX
	 * 		  - #PIXEL_OUT_OF_SYNC
	 * 		  .
	 * 		  A stalled pixel does not update its measurement data and keeps the
	 * 		  previous values. If the issue is resolved, the stall disappears and
	 * 		  the pixel is updating again. */
	PIXEL_STALLED = 1U << 7U

} argus_px_status_t;

/*!***************************************************************************
 * @brief	The evaluated measurement results per pixel.
 * @details	This structure contains the evaluated data for a single pixel.\n
 * 			If the amplitude is 0, the pixel is turned off or has invalid data.
 *****************************************************************************/
typedef struct
{
	/*! Range Values from the device in meter. It is the actual distance before
	 *  software adjustments/calibrations. */
	q9_22_t Range;

	/*! Phase Values from the device in units of PI, i.e. 0 ... 2. */
	uq1_15_t Phase;

	/*! Amplitudes of measured signals in LSB.
	 *  Special values: 0 == Pixel Off, 0xFFFF == Overflow/Error */
	uq12_4_t Amplitude;

	/*! Pixel status; determines if the pixel is disabled, saturated, ..
	 *  See the \link #argus_px_status_t pixel status flags\endlink for more
	 *  information. */
	argus_px_status_t Status;

} argus_pixel_t;

/*! Pixel Crosstalk Vector */
typedef struct
{
	/*! Crosstalk Vector - Sine component.
	 *  Special Value: Q11_4_MIN == not available */
	q11_4_t dS;

	/*! Crosstalk Vector - Cosine component.
	 *  Special Value: Q11_4_MIN == not available */
	q11_4_t dC;

} xtalk_t;


/*! @} */
#endif /* ARGUS_PX_H */
