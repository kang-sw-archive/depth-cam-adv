/*************************************************************************//**
 * @file
 * @brief    	This file is part of the Argus hardware API.
 * @details		Defines the generic Argus API measurement parameter structure.
 *
 * @copyright	Copyright (c) 2016-2018, Avago Technologies GmbH.
 * 				All rights reserved.
 *****************************************************************************/

#ifndef ARGUS_MEAS_H
#define ARGUS_MEAS_H

/*!***************************************************************************
 * @defgroup 	argusmeas Measurement API
 * @ingroup		argusapi
 * @brief 		The measurement frame configuration.
 * @details		This module contains definitions about the measurement frame
 * 				configuration.
 * @addtogroup 	argusmeas
 * @{
 *****************************************************************************/

#include "argus_dca.h"
#include "argus_def.h"

/*! Size of the raw data in bytes. */
#define ARGUS_RAW_DATA_SIZE 396U // 3 bytes * 33 channels * 4 phases

/*! The number channels for auxiliary measurements readout. */
#define ARGUS_AUX_CHANNEL_COUNT (5U)

/*! Size of the auxiliary data in bytes. */
#define ARGUS_AUX_DATA_SIZE (3U * ARGUS_AUX_CHANNEL_COUNT) // 3 bytes * x channels * 1 phase

/*!***************************************************************************
 * @brief	The Argus measurement configuration structure.
 * @details	The portion of the configuration data that belongs to the
 * 			measurement cycle. I.e. the data that defines a measurement frame.
 *****************************************************************************/
typedef struct Argus_MeasurementFrame
{
	/*! ADC channel enabled mask for the first
	 *  channels 0 .. 31 (active pixels channels). */
	uint32_t PxEnMask;

	/*! ADS channel enabled mask for the remaining
	 *  channels 31 .. 63 (miscellaneous values). */
	uint32_t ChEnMask;

	/*! Auxiliary sample count.
	 * 0 determines the auxiliary task was disabled. */
	uint16_t AuxSampleCount;

	/*! The constant range factor that evaluates the range out of the phase for the
	 *  current device configuration.
	 *  RangeFactor = (c0 / (4 * f_mod)) in QU10.22 format. */
	uq10_22_t RangeFactor;

	/*! Pattern count per sample in uq10.6 format.
	 *  Determines the analog integration depth. */
	uq10_6_t AnalogIntegrationDepth;

	/*! Sample count per phase/frame.
	 *  Determines the digital integration depth. */
	uint16_t DigitalIntegrationDepth;

	/*! Laser current per sample in mA.
	 *  Determines the optical output power. */
	uq12_4_t OutputPower;

	/*! Charge pump voltage per sample in LSB.
	 *  Determines the pixel gain.  */
	uint8_t PixelGain;

	/*! Phase count per frame measurements. */
	uint8_t PhaseCount;

	/*! The invalid portion of the sample values in % of the total range.
	 *  E.g. 20% determines the 10% on upper and lower side are tagged as
	 *  invalid. 0% to disable. */
	uq0_8_t SampleInvalidityScope;

	/*! Determines the current evaluation strategy. */
	uint8_t EvalMethod;

	/*! The current measurement mode. */
	argus_mode_t MeasurementMode;

	/*! Determines the status of the DCA module.
	 *  Used for obtaining calibration data. */
	argus_dca_state_t DCAState;

} argus_meas_frame_t;

/*!***************************************************************************
 * @brief	The raw measurement data buffer type.
 * @details Contains all data as read from the devices EME value register.
 * 			Additionally it contains information about the measurement
 * 			conditions such as the trigger time stamp or some configuration
 * 			parameters.
 *****************************************************************************/
typedef struct ADS_Value_Buffer
{
	/*! The \link #status_t status\endlink of the current measurement frame.
	 *   - 0 (i.e. #STATUS_OK) for a good measurement signal.
	 *   - > 0 for warnings and weak measurement signal.
	 *   - < 0 for errors and invalid measurement signal. */
	status_t Status;

	/*! Time in milliseconds (measured since the last MCU startup/reset)
	 *  when the measurement was triggered. */
	ltc_t TimeStamp;

	/*! The configuration for the current measurement frame. */
	argus_meas_frame_t Frame;

	/*! Raw unmapped ADC results from the device.
	 *  Data from main measurement task. */
	uint8_t Data[ARGUS_RAW_DATA_SIZE + 1U]; // includes address byte

	/*! Raw unmapped ADC results from the device.
	 *  Data from auxiliary measurement task. */
	uint8_t AuxData[ARGUS_AUX_DATA_SIZE + 1U]; // includes address byte

} ads_value_buf_t;

/*! @} */
#endif /* ARGUS_MEAS_H */
