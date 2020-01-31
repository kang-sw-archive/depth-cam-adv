/*************************************************************************//**
 * @file
 * @brief		This file is part of the Argus hardware API.
 * @details		Provides trigonometric functions applied to fixed point data types.
 * 
 * @copyright	Copyright c 2016-2018, Avago Technologies GmbH.
 * 				All rights reserved.
 *****************************************************************************/

#ifndef FP_TRIG_H
#define FP_TRIG_H

#include "fp_def.h"

/*!***************************************************************************
 * @addtogroup 	fixedpoint
 * @{
 *****************************************************************************/

/*! Enabling the triangular functions with 8-bit return values. */
#define USING_08BIT_VERSION 0

/*! Enabling the triangular functions with 16-bit return values. */
#define USING_16BIT_VERSION 0

#if USING_08BIT_VERSION

/*!***************************************************************************
 * @brief 	Sine function
 * @details	Lookup table base implementation of sine. Input value is given in
 * 			units of pi and is formated as UQ1.7. Therefore the range is
 * 			limited to [ 0 ... 2 ). The result is given as Q0.7.
 * 			The output error is between -7.813E-03 .. 3.831E-03.
 *
 * @param 	x : Phase in units of PI and UQ1.7 format.
 * @return  	Sine value in Q0.7 format. Note the missing 1.00 in this
 * 				format and the corresponding error.
 *****************************************************************************/
q0_7_t fp_sin7(uq1_7_t x);

/*!***************************************************************************
 * @brief 	Cosine function
 * @details	Lookup table base implementation of sine. Input value is given in
 * 			units of pi and is formated as UQ1.7. Therefore the range is
 * 			limited to [ 0 ... 2 ). The result is given as Q0.7.
 * 			The output error is between -7.813E-03 .. 3.831E-03.
 *
 * @param 	x : Phase in units of PI and UQ1.7 format.
 * @return  	Cosine value in Q0.7 format. Note the missing 1.00 in this
 * 				format and the corresponding error.
 *****************************************************************************/
q0_7_t fp_cos7(uq1_7_t x);

#endif

#if USING_16BIT_VERSION

/*!***************************************************************************
 * @brief 	Sine function
 * @details	Lookup table base implementation of sine. Input value is given in
 * 			units of pi and is formated as UQ1.7. Therefore the range is
 * 			limited to [ 0 ... 2 ). The result is given as Q0.15.
 * 			The output error is between -3.052E-05 .. 1.512E-05.
 *
 * @param 	x : Phase in units of PI and UQ1.7 format.
 * @return  	Sine value in Q0.15 format. Note the missing 1.00 in this
 * 				format and the corresponding error.
 *****************************************************************************/
q0_15_t fp_sin15(uq1_7_t x);

/*!***************************************************************************
 * @brief 	Cosine function
 * @details	Lookup table base implementation of sine. Input value is given in
 * 			units of pi and is formated as UQ1.7. Therefore the range is
 * 			limited to [ 0 ... 2 ). The result is given as Q0.15.
 * 			The output error is between -3.052E-05 .. 1.512E-05.
 *
 * @param 	x : Phase in units of PI and UQ1.7 format.
 * @return  	Cosine value in Q0.15 format. Note the missing 1.00 in this
 * 				format and the corresponding error.
 *****************************************************************************/
q0_15_t fp_cos15(uq1_7_t x);

#endif

/*! @} */

#endif /* FP_TRIG_H */
