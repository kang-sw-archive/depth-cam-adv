/*************************************************************************//**
 * @file
 * @brief    	This file is part of the AFBR-S50 API.
 * @details		Defines macros to work with pixel and ADC channel masks.
 * 
 * @copyright	Copyright c 2016-2019, Avago Technologies GmbH.
 * 				All rights reserved.
 *****************************************************************************/

#ifndef ARGUS_MSK_H
#define ARGUS_MSK_H

/*!***************************************************************************
 * @addtogroup 	arguscfg
 * @{
 *****************************************************************************/

#include "api/argus_def.h"
#include "utility/int_math.h"

/*!*****************************************************************************
 * @brief	Macro to determine the channel number of an specified Pixel.
 * @param	x The x index of the pixel.
 * @param	y The y index of the pixel.
 * @return	The channel number n of the pixel.
 ******************************************************************************/
#define PIXEL_XY2N(x, y) ((uint8_t)((y) - ((x) << 1) + (14 * (((y) >> 1) + 1))))

/*!*****************************************************************************
 * @brief	Macro to determine the x index of an specified Pixel channel.
 * @param	n The channel number of the pixel.
 * @return	The x index number of the pixel.
 ******************************************************************************/
#define PIXEL_N2X(n) ((uint8_t)(((31 - (n)) >> 1) + ((uint8_t)(((n) >> 4) - 1) << 3)))

/*!*****************************************************************************
 * @brief	Macro to determine the y index of an specified Pixel channel.
 * @param	n The channel number of the pixel.
 * @return	The y index number of the pixel.
 ******************************************************************************/
#define PIXEL_N2Y(n) ((uint8_t)(((n) & 1U) + (((n) >> 4) << 1)))

/*!*****************************************************************************
 * @brief	Macro to determine if a ADC Pixel channel was enabled from a pixel mask.
 * @param	msk The 32-bit pixel mask
 * @param	ch The channel number of the pixel.
 * @return 	True if the pixel channel n was enabled, false elsewise.
 ******************************************************************************/
#define PIXELN_ISENABLED(msk, ch) (((msk) >> (ch)) & 0x01U)

/*!*****************************************************************************
 * @brief	Macro enables an ADC Pixel channel in a pixel mask.
 * @param	msk The 32-bit pixel mask
 * @param	ch The channel number of the pixel.
 ******************************************************************************/
#define PIXELN_ENABLE(msk, ch) ((msk) |= (0x01U << (ch)))

/*!*****************************************************************************
 * @brief	Macro disables an ADC Pixel channel in a pixel mask.
 * @param	msk The 32-bit pixel mask
 * @param	ch The channel number of the pixel.
 ******************************************************************************/
#define PIXELN_DISABLE(msk, ch) ((msk) &= (~(0x01U << (ch))))

/*!*****************************************************************************
 * @brief	Macro to determine if an ADC Pixel channel was enabled from a pixel mask.
 * @param	msk 32-bit pixel mask
 * @param	x x index of the pixel.
 * @param	y y index of the pixel.
 * @return 	True if the pixel (x,y) was enabled, false elsewise.
 ******************************************************************************/
#define PIXELXY_ISENABLED(msk, x, y) (PIXELN_ISENABLED(msk, PIXEL_XY2N(x, y)))

/*!*****************************************************************************
 * @brief	Macro enables an ADC Pixel channel in a pixel mask.
 * @param	msk 32-bit pixel mask
 * @param	x x index of the pixel.
 * @param	y y index of the pixel.
 ******************************************************************************/
#define PIXELXY_ENABLE(msk, x, y) (PIXELN_ENABLE(msk, PIXEL_XY2N(x, y)))

/*!*****************************************************************************
 * @brief	Macro disables an ADC Pixel channel in a pixel mask.
 * @param	msk 32-bit pixel mask
 * @param	x x index of the pixel.
 * @param	y y index of the pixel.
 ******************************************************************************/
#define PIXELXY_DISABLE(msk, x, y) (PIXELN_DISABLE(msk, PIXEL_XY2N(x, y)))

/*!*****************************************************************************
 * @brief	Macro to determine if a ADC channel was enabled from a channel mask.
 * @param	msk 32-bit channel mask
 * @param	ch channel number of the ADC channel.
 * @return 	True if the ADC channel n was enabled, false elsewise.
 ******************************************************************************/
#define CHANNELN_ISENABLED(msk, ch) (((msk) >> ((ch) - 32U)) & 0x01U)

/*!*****************************************************************************
 * @brief	Macro to determine if a ADC channel was enabled from a channel mask.
 * @param	msk 32-bit channel mask
 * @param	ch channel number of the ADC channel.
 * @return 	True if the ADC channel n was enabled, false elsewise.
 ******************************************************************************/
#define CHANNELN_ENABLE(msk, ch) ((msk) |= (0x01U << ((ch) - 32U)))

/*!*****************************************************************************
 * @brief	Macro to determine if a ADC channel was enabled from a channel mask.
 * @param	msk 32-bit channel mask
 * @param	ch channel number of the ADC channel.
 * @return 	True if the ADC channel n was enabled, false elsewise.
 ******************************************************************************/
#define CHANNELN_DISABLE(msk, ch) ((msk) &= (~(0x01U << ((ch) - 32U))))

/*!*****************************************************************************
 * @brief	Macro to determine the number of enabled channels via a popcount
 * 			algorithm.
 * @param	pxmsk 32-bit pixel mask
 * @param	chmsk 32-bit channel mask
 * @return 	The count of enabled ADC channels.
 ******************************************************************************/
#define CHANNEL_COUNT(pxmsk, chmsk) (popcount(pxmsk) + popcount(chmsk))

/*! @} */
#endif /* ARGUS_MSK_H */
