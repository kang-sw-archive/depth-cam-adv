/*************************************************************************//**
 * @file
 * @brief    	This file is part of the Argus hardware API.
 * @details		Defines the generic Argus API calibration data structure.
 *
 * @copyright	Copyright (c) 2016-2018, Avago Technologies GmbH.
 * 				All rights reserved.
 *****************************************************************************/

#ifndef ARGUS_CAL_H
#define ARGUS_CAL_H

/*!***************************************************************************
 * @defgroup 	arguscal Calibration API
 * @ingroup		argusapi
 * @brief 		This module manages the calibration specific data.
 * @details		The calibration concept of the Time-of-Flight measurement device
 * 				is made up of three parts:
 * 				 - Factory calibration data which is provided in the devices
 * 				   EEPROM. This data is read and applied on initialization.
 * 				 - Online calibration is executed along with the actual distance
 * 				   measurements. Therefore, ambient information, e.g. temperature
 * 				   or voltage levels, are gathered during measurements. The
 * 				   information is then utilized to adjust the device configuration
 * 				   and evaluation algorithms accordingly.
 * 				 - User calibration parameters are applied on top of this. These
 * 				   parameters are provided in order to adopt the device to the
 * 				   user application, e.g. cover glass calibration. These parameters
 * 				   must be set (if required) by the given API.
 * 				 .
 * @note		This module is not implemented yet!
 * @addtogroup 	arguscal
 * @{
 *****************************************************************************/

#include "argus_def.h"

/*! Creates the bit mask to check if calibration data for a given integration
 *  depth is available. */
#define CAL_HASDATA_MASK(intdepth_EXP) ((uint16_t)(1U << (uint8_t)intdepth_EXP))

/*! The maximum RC trimming offset (4 bit signed value). */
#define ARGUS_MAX_RCOFFSET (7)

/*! The maximum RC trimming offset (4 bit signed value). */
#define ARGUS_MIN_RCOFFSET (-8)

/*! The maximum N for the laser offset compensation. */
#define ARGUS_MAX_LASER_OFFSET_N (31)

/*! The minimum N for the laser offset compensation. */
#define ARGUS_MIN_LASER_OFFSET_N (1)

/*! The maximum gain value for APD calibration. */
#define ARGUS_CAL_APD_GAIN_MAX (100)

/*! The minimum gain value for APD calibration. */
#define ARGUS_CAL_APD_GAIN_MIN (10)

/*! The maximum voltage value in LSB for APD calibration. */
#define ARGUS_CAL_APD_V_MAX (63)

/*! The step size for the LOP trimming value. */
#define ARGUS_CAL_LOP_TRIM_STEP ((uq16_16_t)(1638)) // 0.025

/*! The step size for the range offset value. */
#define ARGUS_CAL_RANGE_OFFSET_STEP ((q9_22_t)(4194)) // 0.05

/*!***************************************************************************
 * @brief	EEPROM Compensation Parameters.
 * @details	Contains EEPROM calibration data.
 *****************************************************************************/
typedef struct Argus_Calibration_EEPROM
{
	/*! Enables the EEPROM data readout. If true, the calibration trimming
	 *  values are determined by the EERPOM. If false, the values can be
	 *  specified by the user. */
	bool EnableEEPROMReadout;

	/*! RC Oscillator Trimming (RCO_TRIM) [LSB] */
	int8_t RCOTrim;

	/*! Actual Frequency (FREQ_OFFSET) [‰ (per mill)] */
	int8_t FrequencyOffset;

	/*! Laser LOP correction factor, RT trim (LOP_TRIM) */
	int8_t LOPTrim;

	/*! Temperature Sensor Trimming (TEMP_TRIM) */
	int16_t TemperatureTrim;

	/*! Golden Pixel GP (GOLDEN_PX);
	 *
	 * 0 = 5/2 (default)
	 * 1 = 5/1
	 * 2 = 5/3
	 * 3 = 6/1
	 * 4 = 6/2
	 * 5 = 6/3
	 * 6 = 4/1
	 * 7 = 4/2 */
	uint8_t GoldenPixel;

	/*! Offset correction short range mode (RANGE_OFFSET) [mm] */
	int8_t RangeOffsetTrim;

	/*! APD minimum Voltage, RT trim (APD_VMIN_TRIM) */
	int8_t APDVminTrim;

	/*! APD maximum Voltage, RT trim (APD_VMAX_TRIM) */
	int8_t APDVmaxTrim;

	/*! APD gain factor (APD_GAIN_FACTOR) */
	int8_t APDGainFactor;

	/*! APD: Distance offset at Vmin, RT trim (APD_RANGE_OFFSET) */
	int8_t APDRangeOffset;

	/*! APD: Distance offset interpolation (APD_RANGE_BOW) */
	int8_t APDRangeBow;

	/*! VCSEL: Distance offset at Imin, RT trim (VCSEL_IMIN_OFFSET) */
	int8_t VCSELRangeOffset;

} argus_cal_eeprom_t;

/*!***************************************************************************
 * @brief	Generic Compensation Parameters.
 * @details	Contains generic calibration data.
 *****************************************************************************/
typedef struct Argus_Calibration_Generic
{
	/*! Generic compensations on/off. */
	bool Enabled;

	/*! Range offset (w/o wrap around effect!) */
	q9_22_t OffsetRange;

	/*! Phase offset (w/ wrap around effect!) */
	uq1_15_t OffsetPhase;

	/*! Range slope correction (an additional correction factor to be
	 *  applied at the Phase-to-Range conversion). */
	uq1_15_t SlopeRange;

	/*! The s/c ratio slope correction value. */
	uq1_15_t SlopeSCValue;

	/*! DCA compensations range offset for the global phase shifted state.
	 * The offset is relative and adds to the generic compensation offset. */
	q9_22_t PhaseShiftOffset;

} argus_cal_generic_t;

/*!***************************************************************************
 * @brief	Crosstalk Compensation Parameters.
 * @details	Contains crosstalk calibration data.
 *****************************************************************************/
typedef struct Argus_Calibration_Crosstalk
{
	/*! Crosstalk compensations on/off. */
	bool Enabled;

	/*! Crosstalk compensations vector. */
	q11_4_t Sxtalk[ARGUS_PIXELS_X][ARGUS_PIXELS_Y][ARGUS_PHASECOUNT >> 1U];

} argus_cal_xtalk_t;

/*!***************************************************************************
 * @brief	Pixel Signal Decompression Parameters.
 * @details	Contains calibration data that belongs to the pixel signal amplitude
 * 			decompression feature.
 *****************************************************************************/
typedef struct Argus_Calibration_Decompression
{
	/*! Amplitude decompression on/off. */
	bool Enabled;

	/*! The amplitude decompression factor @ analog integration depth = 16. */
	uq2_6_t Factor;

} argus_cal_decomp_t;

/*!***************************************************************************
 * @brief	Laser Offset Calibration Parameters.
 * @details	Contains calibration data that belongs to the laser specific
 * 			calibration routines.
 *****************************************************************************/
typedef struct Argus_Calibration_LaserOffset
{
	/*! Laser offset calibration on/off. */
	bool Enabled;

	/*! The nominal range offset in m. */
	q9_22_t RangeOffset;

} argus_cal_laser_t;

/*!***************************************************************************
 * @brief	APD Calibration Parameters.
 * @details	Contains calibration data that belongs to the APD specific
 * 			calibration routines.
 *****************************************************************************/
typedef struct Argus_Calibration_APD
{
	/*! APD calibration on/off. */
	bool Enabled;

	/*! The nominal minimum voltage in LSB.
	 * 	Valid range: 1 .. 63
	 *  0 <= Vmin < Vmax <= 63 */
	uint8_t Vmin;

	/*! The nominal maximum voltage in LSB.
	 * 	Valid range: 1 .. 63
	 *  0 <= Vmin < Vmax <= 63 */
	uint8_t Vmax;

	/*! The nominal gain factor. */
	uq6_2_t GainFactor;

	/*! The nominal range bowing parameter. */
	uq6_2_t RangeBowing;

	/*! The nominal range offset in m. */
	q9_22_t RangeOffset;


	/*! The nominal temperature where the nominal voltage values are calibrated. */
	q11_4_t Temp0;

	/*! The voltage temperature drift value, i.e. the dV/dT coefficient.
	 *  The drift will result in an negative offset:
	 *  V = V0 - dV/dT (T - T0). */
	uq1_7_t VoltageDrift;

} argus_cal_apd_t;

/*!***************************************************************************
 * @brief	Pixel Signal Decompression Parameters.
 * @details	Contains calibration data that belongs to the pixel signal amplitude
 * 			decompression feature.
 *****************************************************************************/
typedef struct Argus_Calibration_Auxiliary
{
	/*! Auxiliary calibration on/off. */
	bool Enabled;

	/*! The temperature sensor nominal value in LSB @ 20°C. */
	uq12_4_t TempNominal;

	/*! The temperature slope in centigrade per LSB and Q1.15 format. */
	uq1_15_t TempSlope;

} argus_cal_aux_t;

/*!***************************************************************************
 * @brief	The Argus calibration structure per measurement mode.
 * @details	The calibration of the Argus device per measurement mode.
 *****************************************************************************/
typedef struct Argus_Calibration_Mode
{
	/*! Generic Compensation Parameters. */
	argus_cal_generic_t Generic;

	/*! Crosstalk Compensation Parameters. */
	argus_cal_xtalk_t Xtalk;

} argus_cal_mode_t;

/*!***************************************************************************
 * @brief	The Argus calibration structure.
 * @details	The calibration of the Argus device.
 *****************************************************************************/
typedef struct Argus_Calibration
{
	/*! APD calibration parameters. */
	argus_cal_apd_t APD;

	/*! Laser calibration parameters. */
	argus_cal_laser_t Laser;

	/*! Pixel Signal Decompression Parameters. */
	argus_cal_decomp_t Decomp;

	/*! Auxiliary measurement readout calibration data. */
	argus_cal_aux_t Auxiliary;

	/*! Calibration per measurement mode. */
	argus_cal_mode_t Mode[ARGUS_MODE_COUNT];

	/*! EEPROM Compensation Parameters. */
	argus_cal_eeprom_t EEPROM;

} argus_calibration_t;

/*! @} */
#endif /* ARGUS_CAL_H */
