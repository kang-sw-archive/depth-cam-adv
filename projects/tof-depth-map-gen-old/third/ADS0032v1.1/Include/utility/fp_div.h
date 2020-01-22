/*************************************************************************//**
 * @file
 * @brief    	This file is part of the Argus hardware API.
 * @details		Provides definitions and basic macros for fixed point data types.
 * 
 * @copyright	Copyright c 2016-2018, Avago Technologies GmbH.
 * 				All rights reserved.
 *****************************************************************************/

#ifndef FP_DIV_H
#define FP_DIV_H

/*!***************************************************************************
 * @addtogroup 	fixedpoint
 * @{
 *****************************************************************************/

#include "fp_def.h"

/*!***************************************************************************
 * @brief	32-bit implementation of an Q15.16 division.
 *
 * @details	Algorithm to evaluate a/b, where b is in Q15.16 format, on a 32-bit
 * 			architecture with maximum precision. This does the division manually,
 * 			and is therefore good for processors that do not have hardware division.
 * 			The result is correctly rounded and given as the input format.
 * 			Division by 0 yields 0 (assert in debug configuration).
 * 			Too high/low results are truncated to max/min values (and asserted
 * 			in debug configuration).
 *
 * 			@see https://code.google.com/archive/p/libfixmath
 *
 * @param 	a :	Numerator in any Qx.y format
 * @param	b : Denominator in Q15.16 format
 * @return	Result = a/b in the same Qx.y format as the input parameter a.
 *****************************************************************************/
int32_t fp_div16(int32_t a, q15_16_t b);

/*! @} */
#endif /* FP_DIV_H */
