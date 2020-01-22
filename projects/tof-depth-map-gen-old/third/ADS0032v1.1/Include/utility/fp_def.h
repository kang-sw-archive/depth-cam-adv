/*************************************************************************//**
 * @file
 * @brief    	This file is part of the Argus hardware API.
 * @details		Provides definitions and basic macros for fixed point data types.
 * 
 * @copyright	Copyright c 2016-2018, Avago Technologies GmbH.
 * 				All rights reserved.
 *****************************************************************************/

#ifndef FP_DEF_H
#define FP_DEF_H

/*!***************************************************************************
 * @defgroup	fixedpoint Fixed Point Math
 * @ingroup		argusutil
 * @brief		A basic math library for fixed point number in the Qx.y fomat.
 * @details		This module contains common fixed point type definitions as
 * 				well as some basic math algorithms. All types are based on
 * 				integer types. The number are defined with the Q number format.
 *
 * 				 - For a description of the Q number format refer to:
 * 					https://en.wikipedia.org/wiki/Q_(number_format)
 * 				 - Another resource for fixed point math in C might be found at
 * 					http://www.eetimes.com/author.asp?section_id=36&doc_id=1287491
 * 				 .
 * @warning		This definitions are not portable and work only with
 * 				little-endian systems!
 * @addtogroup 	fixedpoint
 * @{
 *****************************************************************************/

#include <stdint.h>

/*******************************************************************************
 * UQ6.2
 ******************************************************************************/
/*!***************************************************************************
 * @brief	Unsigned fixed point number: UQ6.2
 * @details	An unsigned fixed point number format based on the 8-bit unsigned
 * 			integer type with 6 integer and 2 fractional bits.
 * 			- Range: 0 .. 63.75
 * 			- Granularity: 0.25
 *****************************************************************************/
typedef uint8_t uq6_2_t;


/*******************************************************************************
 * UQ2.6
 ******************************************************************************/
/*!***************************************************************************
 * @brief	Unsigned fixed point number: UQ2.6
 * @details	An unsigned fixed point number format based on the 8-bit unsigned
 * 			integer type with 2 integer and 6 fractional bits.
 * 			- Range: 0 .. 3.984375
 * 			- Granularity: 0.015625
 *****************************************************************************/
typedef uint8_t uq2_6_t;

/*! The 1/one/unity in UQ2.6 number format. */
#define UQ2_6_ONE ((uq2_6_t)(1U<<6U))

/*******************************************************************************
 * UQ1.7
 ******************************************************************************/
/*!***************************************************************************
 * @brief	Unsigned fixed point number: UQ1.7
 * @details	An unsigned fixed point number format based on the 8-bit unsigned
 * 			integer type with 1 integer and 7 fractional bits.
 * 			- Range: 0 .. 1.9921875
 * 			- Granularity: 0.0078125
 *****************************************************************************/
typedef uint8_t uq1_7_t;

/*******************************************************************************
 * Q0.7
 ******************************************************************************/
/*!***************************************************************************
 * @brief	Signed fixed point number: Q0.7
 * @details	An signed fixed point number format based on the 8-bit integer
 * 			type with 0 integer and 7 fractional bits.
 * 			- Range: -1 .. 0.9921875
 * 			- Granularity: 0.0078125
 *****************************************************************************/
typedef int8_t q0_7_t;

/*******************************************************************************
 * UQ0.8
 ******************************************************************************/
/*!***************************************************************************
 * @brief	Unsigned fixed point number: UQ0.8
 * @details	An unsigned fixed point number format based on the 8-bit unsigned
 * 			integer type with 1 integer and 7 fractional bits.
 * 			- Range: 0 .. 0.99609375
 * 			- Granularity: 0.00390625
 *****************************************************************************/
typedef uint8_t uq0_8_t;

/*******************************************************************************
 * UQ12.4
 ******************************************************************************/
/*!***************************************************************************
 * @brief	Unsigned fixed point number: UQ12.4
 * @details	An unsigned fixed point number format based on the 16-bit unsigned
 * 			integer type with 12 integer and 4 fractional bits.
 * 			- Range: 0 ... 4095.9375
 * 			- Granularity: 0.0625
 *****************************************************************************/
typedef uint16_t uq12_4_t;

/*! Maximum value of UQ12.4 number format. */
#define UQ12_4_MAX ((uq12_4_t)UINT16_MAX)

/*! The 1/one/unity in UQ12.4 number format. */
#define UQ12_4_ONE ((uq12_4_t)(1U<<4U))

/*******************************************************************************
 * Q11.4
 ******************************************************************************/
/*!***************************************************************************
 * @brief	Signed fixed point number: Q11.4
 * @details	An signed fixed point number format based on the 16-bit signed
 * 			integer type with 11 integer and 4 fractional bits.
 * 			- Range: -2048 ... 2047.9375
 * 			- Granularity: 0.0625
 *****************************************************************************/
typedef int16_t q11_4_t;

/*! The 1/one/unity in UQ11.4 number format. */
#define UQ11_4_ONE ((q11_4_t)(1 << 4))

/*! Maximum value of Q11.4 number format. */
#define Q11_4_MAX ((q11_4_t)INT16_MAX)

/*! Minimum value of Q11.4 number format. */
#define Q11_4_MIN ((q11_4_t)INT16_MIN)

/*******************************************************************************
 * UQ10.6
 ******************************************************************************/
/*!***************************************************************************
 * @brief	Unsigned fixed point number: UQ10.6
 * @details	An unsigned fixed point number format based on the 16-bit unsigned
 * 			integer type with 10 integer and 6 fractional bits.
 * 			- Range: 0 ... 1023.984375
 * 			- Granularity: 0.015625
 *****************************************************************************/
typedef uint16_t uq10_6_t;

/*! Maximum value of UQ10.6 number format. */
#define UQ10_6_MAX ((uq10_6_t)UINT16_MAX)

/*! The 1/one/unity in UQ10.6 number format. */
#define UQ10_6_ONE ((uq10_6_t)(1U << 6U))

/*******************************************************************************
 * UQ1.15
 ******************************************************************************/
/*!***************************************************************************
 * @brief	Unsigned fixed point number: UQ1.15
 * @details	An unsigned fixed point number format based on the 16-bit unsigned
 * 			integer type with 1 integer and 15 fractional bits.
 * 			- Range: 0 .. 1.999969
 * 			- Granularity: 0.000031
 *****************************************************************************/
typedef uint16_t uq1_15_t;

/*! The 1/one/unity in UQ1.15 number format. */
#define UQ1_15_ONE ((uq1_15_t)(1U << 15U))

/*******************************************************************************
 * Q0.15
 ******************************************************************************/
/*!***************************************************************************
 * @brief	Signed fixed point number: Q0.15
 * @details	An signed fixed point number format based on the 16-bit integer
 * 			type with 0 integer and 15 fractional bits.
 * 			- Range: -1 .. 0.999969482
 * 			- Granularity: 0.000030518
 *****************************************************************************/
typedef int16_t q0_15_t;

/*******************************************************************************
 * Q2.13
 ******************************************************************************/
/*!***************************************************************************
 * @brief	Signed fixed point number: Q2.13
 * @details	An signed fixed point number format based on the 16-bit integer
 * 			type with 2 integer and 13 fractional bits.
 * 			- Range: -4 .. 3.99987793
 * 			- Granularity: 0.00012207
 *****************************************************************************/
typedef int16_t q2_13_t;

/*******************************************************************************
 * Q13.2
 ******************************************************************************/
/*!***************************************************************************
 * @brief	Signed fixed point number: Q13.2
 * @details	An signed fixed point number format based on the 16-bit integer
 * 			type with 13 integer and 2 fractional bits.
 * 			- Range: -8192 .. 8191.75
 * 			- Granularity: 0.25
 *****************************************************************************/
typedef int16_t q13_2_t;

/*******************************************************************************
 * UQ16.16
 ******************************************************************************/
/*!***************************************************************************
 * @brief	Unsigned fixed point number: UQ16.16
 * @details	An unsigned fixed point number format based on the 32-bit unsigned
 * 			integer type with 16 integer and 16 fractional bits.
 * 			- Range: 0 ... 65535.999984741
 * 			- Granularity: 0.000015259
 *****************************************************************************/
typedef uint32_t uq16_16_t;

/*! The 1/one/unity in UQ16.16 number format. */
#define UQ16_16_ONE ((uq16_16_t)(1U << 16U))

/*! Maximum value of UQ16.16 number format. */
#define UQ16_16_MAX ((uq16_16_t)UINT32_MAX)

/*******************************************************************************
 * Q15.16
 ******************************************************************************/
/*!***************************************************************************
 * @brief	Signed fixed point number: Q15.16
 * @details	An signed fixed point number format based on the 32-bit integer
 * 			type with 15 integer and 16 fractional bits.
 * 			- Range: -32768 .. 32767.99998
 * 			- Granularity: 1.52588E-05
 *****************************************************************************/
typedef int32_t q15_16_t;

/*! The 1/one/unity in Q15.16 number format. */
#define Q15_16_ONE ((q15_16_t)(1U << 16U))

/*! Maximum value of Q15.16 number format. */
#define Q15_16_MAX ((q15_16_t)INT32_MAX)

/*! Minimum value of Q15.16 number format. */
#define Q15_16_MIN ((q15_16_t)INT32_MIN)

/*******************************************************************************
 * UQ10.22
 ******************************************************************************/
/*!***************************************************************************
 * @brief	Unsigned fixed point number: UQ10.22
 * @details	An unsigned fixed point number format based on the 32-bit unsigned
 * 			integer type with 10 integer and 22 fractional bits.
 * 			- Range: 0 ... 1023.99999976158
 * 			- Granularity: 2.38418579101562E-07
 *****************************************************************************/
typedef uint32_t uq10_22_t;

/*******************************************************************************
 * Q9.22
 ******************************************************************************/
/*!***************************************************************************
 * @brief	Signed fixed point number: Q9.22
 * @details	An signed fixed point number format based on the 32-bit integer
 * 			type with 9 integer and 22 fractional bits.
 * 			- Range: -512 ... 511.9999998
 * 			- Granularity: 2.38418579101562E-07
 *****************************************************************************/
typedef int32_t q9_22_t;

/*! The 1/one/unity in Q9.22 number format. */
#define Q9_22_ONE ((q9_22_t)(1U<<22U))

/*! @} */
#endif /* FP_DEF_H */
