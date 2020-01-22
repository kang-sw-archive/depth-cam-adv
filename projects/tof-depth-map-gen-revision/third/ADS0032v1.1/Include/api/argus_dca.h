/*************************************************************************//**
 * @file
 * @brief    	This file is part of the Argus hardware API.
 * @details		Defines algorithms for dynamic configurations adaption feature.
 * 
 * @copyright	Copyright (c) 2016-2018, Avago Technologies GmbH.
 * 				All rights reserved.
 *****************************************************************************/

#ifndef ARGUS_CFG_DCA_H
#define ARGUS_CFG_DCA_H

/*!***************************************************************************
 * @defgroup 	argusdca Dynamic Configuration Adaption (DCA) API
 * @ingroup		arguscfg
 * @brief		This module contains algorithms to dynamically adopt the device
 * 				configuration to the ambient conditions.
 * @details		The DCA contains an algorithms that detect ambient conditions
 * 				and adopt the device configuration to the changing parameters
 * 				dynamically while operating the sensor. This is achieved by
 * 				rating the currently received signal quality and changing the
 * 				device configuration accordingly to the gathered information
 * 				from the current measurement frame results before the next
 * 				integration cycle starts.
 *
 * 				The DCA consists of the following features:
 * 			 	- Static or dynamic mode. The first is utilizing the nominal
 * 			 	  values while the latter is dynamically adopting between min.
 * 			 	  and max. value and starting form the nominal values.
 * 			 	- Analog Integration Depth Adaption down to single pulses.
 * 			 	- Optical Output Power Adaption
 * 			 	- Pixel Input Gain Adaption
 * 			 	- Digital Integration Depth Adaption
 * 			 	- Dynamic Global Phase Shift Injection.
 * 			 	- All that features are heeding the Laser Safety limits.
 * 				.
 * @addtogroup 	argusdca
 * @{
 *****************************************************************************/

#include "argus_def.h"

/*! The maximum analog integration depth in UQ10.6 format,
 * i.e. the maximum pattern count per sample. */
#define ARGUS_CFG_DCA_DEPTH_MAX 	((uq10_6_t)(ADS_SEQCT_N_MASK << (6U - ADS_SEQCT_N_SHIFT)))

/*! The minimum analog integration depth in UQ10.6 format,
 *  i.e. the minimum pattern count per sample. */
#define ARGUS_CFG_DCA_DEPTH_MIN 	((uq10_6_t)(1U)) // 1/64, i.e. 1/2 nibble

/*! The maximum optical output power, i.e. the maximum VCSEL 1 high current in LSB. */
#define ARGUS_CFG_DCA_POWER_MAX_LSB (ADS_LASET_VCSEL_HC1_MASK >> ADS_LASET_VCSEL_HC1_SHIFT)

/*! The minimum optical output power, i.e. the minimum VCSEL 1 high current in mA. */
#define ARGUS_CFG_DCA_POWER_MIN_LSB (1)

/*! The maximum optical output power, i.e. the maximum VCSEL 1 high current in LSB. */
#define ARGUS_CFG_DCA_POWER_MAX		(ADS0032_HIGH_CURRENT_LSB2MA(ARGUS_CFG_DCA_POWER_MAX_LSB + 1))

/*! The minimum optical output power, i.e. the minimum VCSEL 1 high current in mA. */
#define ARGUS_CFG_DCA_POWER_MIN 	(1)

/*! The minimum amplitude threshold value. */
#define ARGUS_CFG_DCA_ATH_MIN		(1U << 6U)

/*! The maximum amplitude threshold value. */
#define ARGUS_CFG_DCA_ATH_MAX		(0xFFFFU)

/*! The minimum saturated pixel threshold value. */
#define ARGUS_CFG_DCA_PXTH_MIN		(1U)

/*! The maximum saturated pixel threshold value. */
#define ARGUS_CFG_DCA_PXTH_MAX		(33U)


/*!***************************************************************************
 * @brief	The dynamic configuration algorithm enable flags.
 *****************************************************************************/
typedef enum Argus_CFG_DCA_Enable
{
	/*! DCA is disabled and will be completely skipped. */
	DCA_ENABLE_OFF = 0,

	/*! DCA is enabled and will dynamically adjust the device configuration. */
	DCA_ENABLE_DYNAMIC = 1,

	/*! DCA is enabled and will apply the static (nominal) values to the device. */
	DCA_ENABLE_STATIC = -1,

} argus_dca_enable_t;

/*!***************************************************************************
 * @brief	The dynamic configuration algorithm output power stages enumerator.
 *****************************************************************************/
typedef enum Argus_CFG_DCA_PowerStages
{
	/*! Low output power stage. */
	DCA_POWER_LOW = 0,

	/*! Medium low output power stage. */
	DCA_POWER_MEDIUM_LOW = 1,

	/*! Medium high output power stage. */
	DCA_POWER_MEDIUM_HIGH = 2,

	/*! High output power stage. */
	DCA_POWER_HIGH = 3

} argus_dca_power_t;


/*!***************************************************************************
 * @brief	The dynamic configuration algorithm gain stages enumerator.
 *****************************************************************************/
typedef enum Argus_CFG_DCA_GainStages
{
	/*! Low gain stage. */
	DCA_GAIN_LOW = 0,

	/*! Medium low gain stage. */
	DCA_GAIN_MEDIUM_LOW = 1,

	/*! Medium high gain stage. */
	DCA_GAIN_MEDIUM_HIGH = 2,

	/*! High gain stage. */
	DCA_GAIN_HIGH = 3

} argus_dca_gain_t;


/*!***************************************************************************
 * @brief	The dynamic configuration algorithm state mask for the gain stage.
 *****************************************************************************/
#define DCA_STATE_GAIN_MASK (0x03U)

/*!***************************************************************************
 * @brief	The dynamic configuration algorithm state mask for the gain stage.
 *****************************************************************************/
#define DCA_STATE_GAIN_SHIFT (6U)

/*!***************************************************************************
 * @brief	The dynamic configuration algorithm gain stage count.
 *****************************************************************************/
#define DCA_STATE_GAIN_COUNT (4U)

/*!***************************************************************************
 * @brief	Getter for the dynamic configuration algorithm gain stage.
 *****************************************************************************/
#define DCA_STATE_GAIN_GET(state) (((state) >> DCA_STATE_GAIN_SHIFT) & DCA_STATE_GAIN_MASK)


/*!***************************************************************************
 * @brief	The dynamic configuration algorithm state mask for the power stage.
 *****************************************************************************/
#define DCA_STATE_POWER_MASK (0x03U)

/*!***************************************************************************
 * @brief	The dynamic configuration algorithm state mask for the power stage.
 *****************************************************************************/
#define DCA_STATE_POWER_SHIFT (4U)

/*!***************************************************************************
 * @brief	The dynamic configuration algorithm power stage count.
 *****************************************************************************/
#define DCA_STATE_POWER_COUNT (4U)

/*!***************************************************************************
 * @brief	Getter for the dynamic configuration algorithm power stage.
 *****************************************************************************/
#define DCA_STATE_POWER_GET(state) (((state) >> DCA_STATE_POWER_SHIFT) & DCA_STATE_POWER_MASK)


/*!***************************************************************************
 * @brief	The dynamic configuration algorithm state flags.
 *****************************************************************************/
typedef enum Argus_CFG_DCA_State
{
	/*! DCA is in normal stage. */
	DCA_STATE_NORMAL = 0,

	/*! DCA is in phase shifted mode. */
	DCA_STATE_PHASESHIFTED = 1,

	/*! DCA is in high gain stage. */
	DCA_STATE_LOW_GAIN = DCA_GAIN_LOW << DCA_STATE_GAIN_SHIFT,

	/*! DCA is in high gain stage. */
	DCA_STATE_MED_LOW_GAIN = DCA_GAIN_MEDIUM_LOW << DCA_STATE_GAIN_SHIFT,

	/*! DCA is in high gain stage. */
	DCA_STATE_MED_HIGH_GAIN = DCA_GAIN_MEDIUM_HIGH << DCA_STATE_GAIN_SHIFT,

	/*! DCA is in low gain stage. */
	DCA_STATE_HIGH_GAIN = DCA_GAIN_HIGH << DCA_STATE_GAIN_SHIFT,

	/*! DCA is in high gain stage. */
	DCA_STATE_LOW_POWER = DCA_GAIN_LOW << DCA_STATE_POWER_SHIFT,

	/*! DCA is in high gain stage. */
	DCA_STATE_MED_LOW_POWER = DCA_GAIN_MEDIUM_LOW << DCA_STATE_POWER_SHIFT,

	/*! DCA is in high gain stage. */
	DCA_STATE_MED_HIGH_POWER = DCA_GAIN_MEDIUM_HIGH << DCA_STATE_POWER_SHIFT,

	/*! DCA is in low gain stage. */
	DCA_STATE_HIGH_POWER = DCA_GAIN_HIGH << DCA_STATE_POWER_SHIFT,

} argus_dca_state_t;



/*!***************************************************************************
 * @brief	Dynamic Configuration Adaption (DCA) Parameters.
 * @details	DCA contains:
 * 			 - Static or dynamic mode. The first is utilizing the nominal values
 * 			   while the latter is dynamically adopting between min. and max.
 * 			   value and starting form the nominal values.
 * 			 - Analog Integration Depth Adaption down to single pulses.
 * 			 - Optical Output Power Adaption
 * 			 - Pixel Input Gain Adaption
 * 			 - Digital Integration Depth Adaption
 * 			 - Dynamic Global Phase Shift Injection.
 * 			 - All that features are heeding the Laser Safety limits.
 * 			 .
 *****************************************************************************/
typedef struct Argus_CFG_DCA
{
	/*! Enables the automatic configuration adaption features.
	 *  Enables the dynamic part if #DCA_ENABLE_DYNAMIC and the static only if
	 *  #DCA_ENABLE_STATIC. If set to DCA_ENABLE_OFF, the DCA is completely
	 *  skipped and the static register values are considered which is
	 *  recommended for advanced debugging only. */
	argus_dca_enable_t Enabled;

	/*! The threshold value of saturated pixels that causes a linear reduction
	 *  of the integration energy, i.e. if the number of saturated pixels are
	 *  larger or equal to this value, the integration energy will be reduced
	 *  by a single step (one pattern if the current integration depth is > 1,
	 *  one pulse if the current integration depth is <= 1 or one power LSB for
	 *  the optical power range).
	 *
	 *  Valid values: 1, ..., 33; (use 33 to disable the linear decrease)
	 *  Note that the linear value must be smaller or equal to the exponential
	 *  value. To sum up, it must hold:
	 *  1 <= SatPxThLin <= SatPxThExp <= SatPxThRst <= 33 */
	uint8_t SatPxThLin;

	/*! The threshold number of saturated pixels that causes a exponential
	 *  reduction of the integration energy, i.e. if the number of saturated
	 *  pixels is larger or equal to this value, the integration energy will be
	 *  halved.
	 *
	 *  Valid values: 1, ..., 33; (use 33 to disable the exponential decrease)
	 *  Note that the exponential value must be between the linear and reset
	 *  values. To sum up, it must hold:
	 *  1 <= SatPxThLin <= SatPxThExp <= SatPxThRst <= 33 */
	uint8_t SatPxThExp;

	/*! The threshold number of saturated pixels that causes a sudden reset of
	 *  the integration energy to the minimal value, i.e. if the number of
	 *  saturated pixels are larger or equal to this value, the integration
	 *  energy will suddenly be reset to the minimum values. The gain setting
	 *  will stay at the mid value and a decrease happens after the next step
	 *  if still required.
	 *
	 *  Valid values: 1, ..., 33; (use 33 to disable the sudden reset)
	 *  Note that the reset value must be larger or equal to the exponential
	 *  value. To sum up, it must hold:
	 *  1 <= SatPxThLin <= SatPxThExp <= SatPxThRst <= 33 */
	uint8_t SatPxThRst;

	/*! The amplitude to be targeted from the lower regime. If the amplitude
	 *  lower than the target value, a linear increase of integration energy
	 *  will happen in order to optimize for best performance.
	 *
	 *  Valid values: #ARGUS_CFG_DCA_ATH_MIN, ... #ARGUS_CFG_DCA_ATH_MAX or 0
	 *  Set 0 to disable optimization toward the target amplitude.
	 *  Note further that the following condition must hold:
	 *  'MIN' <= AthLow <= Atarget <= AthHigh <= 'MAX' */
	uq12_4_t Atarget;

	/*! The low threshold value for the max. amplitude. If the max. amplitude
	 *  falls below this value, the integration depth will be increases.
	 *
	 *  Valid values: #ARGUS_CFG_DCA_ATH_MIN, ... #ARGUS_CFG_DCA_ATH_MAX
	 *  Note further that the following condition must hold:
	 *  'MIN' <= AthLow <= Atarget <= AthHigh <= 'MAX' */
	uq12_4_t AthLow;

	/*! The high threshold value for the max. amplitude. If the max. amplitude
	 *  exceeds this value, the integration depth will be decreases. Note that
	 *  also saturated pixels will cause a decrease of the integration depth.
	 *
	 *  Valid values: #ARGUS_CFG_DCA_ATH_MIN, ... #ARGUS_CFG_DCA_ATH_MAX
	 *  Note further that the following condition must hold:
	 *  'MIN' <= AthLow <= Atarget <= AthHigh <= 'MAX' */
	uq12_4_t AthHigh;

	/*! Whether to use the average binned amplitude rather than the maximum
	 *  amplitude for the algorithm.
	 *
	 *  Boolean value: Disabled == 0 or Enabled != 0 */
	uint8_t UseAvgAmpl;

	/*! The nominal analog integration depth in UQ10.6 format,
	 *  i.e. the nominal pattern count per sample.
	 *
	 *  Valid values: #ARGUS_CFG_DCA_DEPTH_MIN, ... #ARGUS_CFG_DCA_DEPTH_MAX
	 *  Note further that the following condition must hold:
	 *  'MIN' <= DepthLow <= DepthNom <= DepthHigh <= 'MAX' */
	uq10_6_t DepthNom;

	/*! The minimum analog integration depth in UQ10.6 format,
	 *  i.e. the minimum pattern count per sample.
	 *
	 *  Valid values: #ARGUS_CFG_DCA_DEPTH_MIN, ... #ARGUS_CFG_DCA_DEPTH_MAX
	 *  Note further that the following condition must hold:
	 *  'MIN' <= DepthLow <= DepthNom <= DepthHigh <= 'MAX' */
	uq10_6_t DepthMin;

	/*! The maximum analog integration depth in UQ10.6 format,
	 *  i.e. the maximum pattern count per sample.
	 *
	 *  Valid values: #ARGUS_CFG_DCA_DEPTH_MIN, ... #ARGUS_CFG_DCA_DEPTH_MAX
	 *  Note further that the following condition must hold:
	 *  'MIN' <= DepthMin <= DepthNom <= DepthMax <= 'MAX' */
	uq10_6_t DepthMax;

	/*! The nominal optical output power in mA,
	 *  i.e. the nominal VCSEL_HC1 setting.
	 *
	 *  Valid values: #ARGUS_CFG_DCA_POWER_MIN, ... #ARGUS_CFG_DCA_POWER_MAX
	 *  Note further that the following condition must hold:
	 *  'MIN' <= PowerMin <= PowerNom <= 'MAX' */
	uq12_4_t PowerNom;

	/*! The minimum optical output power in mA,
	 *  i.e. the minimum VCSEL_HC1 setting.
	 *
	 *  Valid values: #ARGUS_CFG_DCA_POWER_MIN, ... #ARGUS_CFG_DCA_POWER_MAX
	 *  Note further that the following condition must hold:
	 *  'MIN' <= PowerMin <= PowerNom <= 'MAX' */
	uq12_4_t PowerMin;

	/*! The nominal pixel gain setting, i.e. the setting for
	 *  nominal/default gain stage.
	 *
	 *  Valid values: 0,..,3: #DCA_GAIN_LOW, ... #DCA_GAIN_HIGH
	 *  Note further that the following condition must hold:
	 *  'MIN' <= GainMin <= GainNom <= GainMax <= 'MAX' */
	argus_dca_gain_t GainNom;

	/*! The minimal pixel gain setting, i.e. the setting for
	 *  minimum gain stage.
	 *
	 *  Valid values: 0,..,3: #DCA_GAIN_LOW, ... #DCA_GAIN_HIGH
	 *  Note further that the following condition must hold:
	 *  'MIN' <= GainMin <= GainNom <= GainMax <= 'MAX' */
	argus_dca_gain_t GainMin;

	/*! The maximum pixel gain setting, i.e. the setting for
	 *  maximum gain stage.
	 *
	 *  Valid values: 0,..,3: #DCA_GAIN_LOW, ... #DCA_GAIN_HIGH
	 *  Note further that the following condition must hold:
	 *  'MIN' <= GainMin <= GainNom <= GainMax <= 'MAX' */
	argus_dca_gain_t GainMax;

	/*! The dynamic phase shift switching area in % of the max. area.
	 *  Determines the area around the critical part around the edges
	 *  in the correlation function to be shifted by introducing a global
	 *  phase shift.
	 *
	 *  All available values from the 8-bit representation are valid.
	 *  The actual percentage value is determined by 100%/256*x.
	 *
	 *  Use 0 to disable the global phase shift feature.
	 *
	 *  Note that a pattern with periodicity of at least 8-bits is required
	 *  to induce a phase shift of 1/8-th of the period, otherwise the value
	 *  is rejected and a zero must be set. */
	uq0_8_t PhaseShiftArea;

	/*! Single Pulse Mode.
	 *
	 *  If a single laser pulse is left from the pattern shortening, the
	 *  correlation pattern is shortened to a single pulse as well in order
	 *  to prevent overloading the pixels.
	 *
	 *  Boolean value, 0: disabled, else: enabled */
	uint8_t SinglePulseMode;

	/*! Power Saving Ratio value.
	 *
	 *  Determines the percentage of the full available frame time that is not
	 *  exploited for digital integration. Thus the device is idle within the
	 *  specified portion of the frame time and does consume less energy.
	 *
	 *  Note that the laser safety might already limit the maximum integration
	 *  depth and the power saving ratio might not take effect for all ambient
	 *  situations. Thus the Power Saving Ratio is to be understood as a minimum
	 *  percentage where the device is idle per frame.
	 *
	 *  The value is a UQ0.8 format that ranges from 0.0 (=0x00) to 0.996 (=0xFF),
	 *  where 0 means no power saving (i.e. feature disabled) and 0xFF determines
	 *  maximum power saving, i.e. the digital integration depth is limited to a
	 *  single sample.
	 *
	 *  Range: 0x00, .., 0xFF; set 0 to disable. */
	uq0_8_t PowerSavingRatio;

} argus_cfg_dca_t;

/*! @} */
#endif /* ARGUS_CFG_DCA_H */
