/*************************************************************************//**
 * @file
 * @brief    	This file is part of the AFBR-S50 API.
 * @details		This file provides an logarithm function for fixed point type.
 * 
 * @copyright	Copyright c 2016-2019, Avago Technologies GmbH.
 * 				All rights reserved.
 *****************************************************************************/

#ifndef FP_LOG_H
#define FP_LOG_H

/*!***************************************************************************
 * @addtogroup 	fixedpoint
 * @{
 *****************************************************************************/

#include "fp_def.h"

/*!***************************************************************************
 * @brief	Calculates the natural logarithm (base e) of an fixed point number.
 *
 * @details Calculates y = ln(x) = log_e(x) in fixed point representation.
 *
 *			Note that the result might not be 100 % accurate and might contain
 *			a small error!
 *
 * 			@see https://www.quinapalus.com/efunc.html
 *
 * @param	x The input parameter in unsigned fixed point format Q15.16.
 * @return	Result y = ln(x) in the UQ16.16 format.
 *****************************************************************************/
q15_16_t fp_log16(uq16_16_t x);

/*! @} */
#endif /* FP_DIV_H */
