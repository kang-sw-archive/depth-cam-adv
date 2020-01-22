/*************************************************************************//**
 * @file
 * @brief    	This file is part of the Argus hardware API.
 * @details		This file provides an exponential function for fixed point type.
 * 
 * @copyright	Copyright c 2016-2018, Avago Technologies GmbH.
 * 				All rights reserved.
 *****************************************************************************/

#ifndef FP_EXP_H
#define FP_EXP_H

/*!***************************************************************************
 * @addtogroup 	fixedpoint
 * @{
 *****************************************************************************/

#include "fp_def.h"

/*! Euler's number, e, in UQ16.16 format. */
#define UQ16_16_E (0x2B7E1U)

/*!***************************************************************************
 * @brief	Calculates the exponential of an fixed point number.
 *
 * @details Calculates y = exp(x) in fixed point representation.
 *
 *			Note that the result might not be 100 % accurate and might contain
 *			a small error!
 *
 * 			@see https://www.quinapalus.com/efunc.html
 *
 * @param 	x :	The input parameter in unsigned fixed point format Q15.16.
 * @return	Result y = exp(x) in the UQ16.16 format.
 *****************************************************************************/
uq16_16_t fp_exp16(q15_16_t x);

/*! @} */
#endif /* FP_DIV_H */
