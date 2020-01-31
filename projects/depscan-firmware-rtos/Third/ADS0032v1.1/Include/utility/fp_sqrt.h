/*************************************************************************//**
 * @file
 * @brief    	This file is part of the Argus hardware API.
 * @details		This file provides a square root function for fixed point type.
 * 
 * @copyright	Copyright c 2016-2018, Avago Technologies GmbH.
 * 				All rights reserved.
 *****************************************************************************/

#ifndef FP_SQRT_H
#define FP_SQRT_H

/*!***************************************************************************
 * @addtogroup 	fixedpoint
 * @{
 *****************************************************************************/
#if 0
#include "fp_def.h"

/*!***************************************************************************
 * @brief	Calculates the square root of an fixed point number.
 *
 * @details
 * 			@see https://en.wikipedia.org/wiki/Integer_square_root
 * 			@see https://github.com/chmike/fpsqrt/blob/master/fpsqrt.c
 *
 *			@see https://code.google.com/archive/p/libfixmath
 *
 * @param 	q :	The input parameter in unsigned fixed point format UQx.y.
 * @param	y : The number of fractional bits of the fixed point format.
 * @return	Result = sqrt(y) in the same Qx.y format as the input parameter q.
 *****************************************************************************/
uint32_t fp_sqrt0(uint32_t q);

uq16_16_t fp_sqrt16(uq16_16_t q);
uq16_16_t fp_sqrt_int_16( uint32_t v );
#endif

/*! @} */
#endif /* FP_DIV_H */
