#pragma once
#include "stm32f4xx_hal.h"
#include <assert.h>
#include <stdbool.h>
#include <uEmbedded/uassert.h>
extern volatile int irq_lock_ct;

static inline void irq_unlock()
{
    uassert(irq_lock_ct > 0);
    if (--irq_lock_ct == 0) {
        __set_BASEPRI(0U);
    }

    // __enable_irq();
}

static inline void irq_lock()
{
    __set_BASEPRI(1U << __NVIC_PRIO_BITS);
    // __disable_irq();
    irq_lock_ct++;
}

static inline bool is_irq_locked() { return irq_lock_ct != 0; }
