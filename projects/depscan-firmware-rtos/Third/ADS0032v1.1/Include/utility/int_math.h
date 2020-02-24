/*************************************************************************//**
 * @file
 * @brief    	This file is part of the AFBR-S50 API.
 * @details		Provides algorithms applied to integer values.
 * 
 * @copyright	Copyright c 2016-2019, Avago Technologies GmbH.
 * 				All rights reserved.
 *****************************************************************************/

#ifndef INT_MATH
#define INT_MATH

/*!***************************************************************************
 * @addtogroup 	misc
 * @{
 *****************************************************************************/

/*! Enables the integer square root function. */
#ifndef INT_SQRT
#define INT_SQRT 0
#endif

#if !TEST
inline
#endif
static unsigned log2_round(unsigned x);

#if !TEST
inline
#endif
static unsigned binary_round(unsigned x);

#if !TEST
inline
#endif
static unsigned popcount(unsigned x);

#if !TEST
inline
#endif
static unsigned ispowoftwo(unsigned x);

#if INT_SQRT
#if !TEST
inline
#endif
static unsigned isqrt(unsigned v);
#endif

/*!***************************************************************************
 * @brief	Log2 with rounding.
 *
 * @details	Finding the nearest power-of-two value s.t. |x - 2^n| becomes
 * 			minimum for all n.
 * 			Special case 0: returns 0;
 *
 * @param	x Input parameter.
 * @return		Nearest exponent n.
 *****************************************************************************/

static unsigned log2_round(unsigned x)
{
	if (!x) return 0;
	unsigned y = x;
	unsigned i = 0;
	while (y >>= 1) i++;
	return (i + ((x >> (i - 1u)) == 3u));
}

/*!***************************************************************************
 * @brief 	Finding the nearest power-of-two value.
 *
 * @details	Implemented s.t. |x - 2^n| becomes minimum for all n.
 * 			Special case 0: returns 0;
 * 			Maximum input: 3037000499; higher number result in overflow! (returns 0)
 *
 * @param	x Input parameter.
 * @return		Nearest power-of-two number, i.e. 2^n.
 *****************************************************************************/

static unsigned binary_round(unsigned x)
{
	if (!x) return 0;
	unsigned shift = log2_round(x);
	if (shift > 31u) return 0;
	else return 1u << shift;
}

/*!***************************************************************************
 * @brief 	Counting bits set in a 32-bit unsigned integer.
 *
 * @details	@see http://graphics.stanford.edu/~seander/bithacks.html
 *
 * @param	x Input parameter.
 * @return		Number of bits set.
 *****************************************************************************/
static unsigned popcount(unsigned x)
{
	// http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
	x = x - ((x >> 1) & 0x55555555);
	x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
	return (((x + (x >> 4)) & 0xF0F0F0F) * 0x1010101) >> 24;
}

/*!***************************************************************************
 * @brief 	Determining if an integer is a power of 2
 *
 * @details	@see http://graphics.stanford.edu/~seander/bithacks.html
 *
 * @param	x Input parameter.
 * @return		True if integer is power of 2.
 *****************************************************************************/
static unsigned ispowoftwo(unsigned x)
{
	return x && !(x & (x - 1));
}

#if INT_SQRT
/*!***************************************************************************
 * @brief 	Calculates the integer square root of x.
 *
 * @details	The integer square root is defined as:
 * 			isqrt(x) = (int)sqrt(x)
 *
 * 			@see https://en.wikipedia.org/wiki/Integer_square_root
 * 			@see https://github.com/chmike/fpsqrt/blob/master/fpsqrt.c
 *
 * @param	x Input parameter.
 * @return		isqrt(x)
 *****************************************************************************/
static unsigned isqrt(unsigned v)
{
	unsigned t, q, b, r;
	r = v;           // r = v - x²
	b = 0x40000000;  // a²
	q = 0;           // 2ax

	while( b > 0 )
	{
		t = q + b;   // t = 2ax + a²
		q >>= 1;     // if a' = a/2, then q' = q/2
		if( r >= t ) // if (v - x²) >= 2ax + a²
		{
			r -= t;  // r' = (v - x²) - (2ax + a²)
			q += b;  // if x' = (x + a) then ax' = ax + a², thus q' = q' + b
		}
		b >>= 2;     // if a' = a/2, then b' = b / 4
	}
	return q;
}
#endif // INT_SQRT

/*! @} */
#endif /* INT_MATH */
