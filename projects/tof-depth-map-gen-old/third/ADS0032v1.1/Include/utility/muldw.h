/*************************************************************************//**
 * @file
 * @brief    	This file is part of the Argus hardware API.
 * @details		Provides algorithms for multiplying long data types
 * 				(uint32_t x uint32_t) on a 32-bit architecture.
 * 
 * @copyright	Copyright c 2016-2018, Avago Technologies GmbH.
 * 				All rights reserved.
 *****************************************************************************/

#ifndef MULDW_H
#define MULDW_H

/*!***************************************************************************
 * @addtogroup 	misc
 * @{
 *****************************************************************************/

#include <stdint.h>

/*!***************************************************************************
 * @brief 	Long multiplication of two signed 32-bit into an 64-bit value on
 * 			32-bit architecture.
 *
 * @details	w (two words) gets the product of u and v (one word each).
 * 			w[0] is the most significant word of the result, w[1] the least.
 * 			(The words are in big-endian order).
 * 			It is derived from Knuth's Algorithm M from [Knu2] section 4.3.1.
 *
 * @see		http://www.hackersdelight.org/hdcodetxt/muldws.c.txt
 *
 * @param 	w : The result (u * v) value given as two signed 32-bit numbers:
 * 				w[0] is the most significant word of the result, w[1] the least.
 * 				(The words are in big-endian order).
 * @param 	u : Left hand side of the multiplication.
 * @param 	v : Right hand side of the multiplication.
 * @return  -
 *****************************************************************************/
void muldws(int32_t w[], int32_t u, int32_t v);

/*!***************************************************************************
 * @brief 	Long multiplication of two unsigned 32-bit into an 64-bit value on
 * 			32-bit architecture.
 *
 * @details	w (two words) gets the product of u and v (one word each).
 * 			w[0] is the most significant word of the result, w[1] the least.
 * 			(The words are in big-endian order).
 * 			It is Knuth's Algorithm M from [Knu2] section 4.3.1.
 * *
 * @see		http://www.hackersdelight.org/hdcodetxt/muldwu.c.txt
 *
 * @param 	w : The result (u * v) value given as two unsigned 32-bit numbers:
 * 				w[0] is the most significant word of the result, w[1] the least.
 * 				(The words are in big-endian order).
 * @param 	u : Left hand side of the multiplication.
 * @param 	v : Right hand side of the multiplication.
 * @return  -
 *****************************************************************************/
void muldwu(uint32_t w[], uint32_t u, uint32_t v);

/*! @} */
#endif /* MULDW_H */
