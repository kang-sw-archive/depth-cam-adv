/*************************************************************************//**
 * @file
 * @brief    	This file is part of the Argus hardware API.
 * @details		Provides averaging algorithms for fixed point data types.
 * 
 * @copyright	Copyright c 2016-2018, Avago Technologies GmbH.
 * 				All rights reserved.
 *****************************************************************************/

#ifndef FP_EMA_H
#define FP_EMA_H

/*!***************************************************************************
 * @addtogroup 	fixedpoint
 * @{
 *****************************************************************************/

#include "fp_def.h"

/*!***************************************************************************
 * @brief	Circular exponentially weighted moving average using UQ1.15 format.
 *
 * @details Evaluates the moving average (exponentially weighted) for circular
 * 			data in UQ1.15 format.
 * 			Circular data is that MAX_VALUE + 1 == MIN_VALUE. For example the
 * 			usual phase information.
 *
 * 			Problem: Due to circularity of phase values, i.e. 0+x and 2PI+x are
 * 			the same, the usual EMA has issues with the wrap around effect.
 * 			Especially for vectors with phase around 0 (or 2PI), two values
 * 			like 0 + x and PI - y are averaged to something around PI instead
 * 			of 0 which would be more correct.
 *
 * 			Solution: Assume that phase jumps of more than PI are not allowed
 * 			or possible. If a deviation of the new value to the smoothed signal
 * 			occurs, it is clear that this stems from the wrap around effect and
 * 			can be catched and correctly handled by the smoothing algorithm.
 *
 * 			Caution: If a target comes immediately into the field of view, phase
 * 			jumps of > PI are indeed possible and volitional. However, the
 * 			averaging break there anyway since the smoothed signal approaches
 * 			only with delay to the correct values. The error made here is, that
 * 			the smoothed signal approaches from the opposite direction. However,
 * 			is approaches even faster since it always takes the shortest
 * 			direction.
 *
 * @param	mean : The previous mean value in UQ1.15 format.
 * @param	x : The current value to be added to the average UQ1.15 format.
 * @param	weight : The EMA weight in UQ0.7 format.
 * @return  The new mean value in UQ1.15 format.
 *****************************************************************************/
uq1_15_t fp_ema15c(uq1_15_t mean, uq1_15_t x, uq0_8_t weight);

/*!***************************************************************************
 * @brief	Exponentially weighted moving average using the Q11.4 format.
 *
 * @details Evaluates the moving average (exponentially weighted) for data in
 * 			Q11.4 format.
 *
 * @param	mean : The previous mean value in Q11.4 format.
 * @param	x : The current value to be added to the average Q11.4 format.
 * @param	weight : The EMA weight in UQ0.7 format.
 * @return  The new mean value in Q11.4 format.
 *****************************************************************************/
q11_4_t fp_ema4(q11_4_t mean, q11_4_t x, uq0_8_t weight);

/*! @} */
#endif /* FP_EMA_H */
