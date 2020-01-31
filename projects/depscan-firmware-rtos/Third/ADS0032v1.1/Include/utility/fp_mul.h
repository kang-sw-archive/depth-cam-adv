/*************************************************************************//**
 * @file
 * @brief    	This file is part of the Argus hardware API.
 * @details		Provides definitions and basic macros for fixed point data types.
 * 
 * @copyright	Copyright c 2016-2018, Avago Technologies GmbH.
 * 				All rights reserved.
 *****************************************************************************/

#ifndef FP_MUL_H
#define FP_MUL_H

/*!***************************************************************************
 * @addtogroup 	fixedpoint
 * @{
 *****************************************************************************/

#include "fp_def.h"

/*!***************************************************************************
 * @brief	64-bit implementation of an multiplication with fixed point format.
 *
 * @details	Algorithm to evaluate a*b, where a and b are arbitrary fixed point
 * 			number of 32-bit width. The algorithm ins considering the sign of
 * 			the input values. The multiplication is done in 64-bit and the
 * 			result is shifted down by the passed shift parameter. The shift
 * 			is executed with correct rounding.
 *
 * 			Note that the result must fit into the 32-bit value. An assertion
 * 			error occurs otherwise (or undefined behavior of no assert available).
 *
 * @param 	u :	The left parameter in Qx1.y1 format
 * @param 	v :	The right parameter in Qx2.y2 format
 * @param	shift : The final right shift (rounding) value.
 * @return	Result = (a*b)>>shift in Qx.(y1+y2-shift) format.
 *****************************************************************************/
int32_t fp_muls(int32_t u, int32_t v, uint_fast8_t shift);

/*!***************************************************************************
 * @brief	64-bit implementation of an multiplication with fixed point format.
 *
 * @details	Algorithm to evaluate a*b, where a and b are arbitrary fixed point
 * 			number of 32-bit width. The multiplication is done in 64-bit and
 * 			the result is shifted down by the passed shift parameter. The shift
 * 			is executed with correct rounding.
 *
 * 			Note that the result must fit into the 32-bit value. An assertion
 * 			error occurs otherwise (or undefined behavior of no assert available).
 *
 * @param 	u :	The left parameter in UQx1.y1 format
 * @param 	v :	The right parameter in UQx2.y2 format
 * @param	shift : The final right shift (rounding) value.
 * @return	Result = (a*b)>>shift in UQx.(y1+y2-shift) format.
 *****************************************************************************/
uint32_t fp_mulu(uint32_t u, uint32_t v, uint_fast8_t shift);

/*! @} */
#endif /* FP_MUL_H */
