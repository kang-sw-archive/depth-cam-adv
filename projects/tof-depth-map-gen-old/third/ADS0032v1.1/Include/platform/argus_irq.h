/*************************************************************************//**
 * @file
 * @brief    	This file is part of the Argus API.
 * @details		This file provides an interface for enabling/disabling interrupts.
 * 
 * @copyright	Copyright c 2016-2018, Avago Technologies GmbH.
 * 				All rights reserved.
 *****************************************************************************/

#ifndef ARGUS_IRQ_H
#define ARGUS_IRQ_H

/*!***************************************************************************
 * @defgroup	argus_irq IRQ: Global Interrupt Control Layer
 * @ingroup		argus_platform
 * @brief		Argus- Global Interrupt Control Layer
 * @details		This module provides functionality to globally enable/disable
 * 				interrupts by turning the I-bit in the CPSR on/off.
 * @addtogroup 	argus_irq
 * @{
 *****************************************************************************/

#ifndef __STATIC_INLINE
#define __STATIC_INLINE static inline
#endif
#include <assert.h> 
#include <cmsis_gcc.h>
#include "arch/irq.h"
/*! Lock level counter value. */

/*!***************************************************************************
 * @brief	Enable IRQ Interrupts
 *
 * @details	Enables IRQ interrupts by clearing the I-bit in the CPSR.
 * 			Can only be executed in Privileged modes.
 * @return	-
 *****************************************************************************/
__attribute__( ( always_inline ) ) static inline void IRQ_UNLOCK(void)
{
    irq_unlock();
}

/*!***************************************************************************
 * @brief	Disable IRQ Interrupts
 *
 * @details	Disables IRQ interrupts by setting the I-bit in the CPSR.
 * 			Can only be executed in Privileged modes.
 * @return	-
 *****************************************************************************/
__attribute__( ( always_inline ) ) static inline void IRQ_LOCK(void)
{
    irq_lock();
}

/*! @} */
#endif // ARGUS_IRQ_H
