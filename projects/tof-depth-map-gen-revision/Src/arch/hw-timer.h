#pragma once
#include <main.h>
#include <stm32f4xx_hal.h>
#include <uEmbedded/timer_logic.h>

extern timer_logic_t g_timer;

#define SYSTEM_TIM      (*(volatile TIM_HandleTypeDef*)&htim2)
#define SYSTEM_TIM_INST ((volatile TIM_TypeDef*)SYSTEM_TIM.Instance)
static inline size_t SYSTEM_TIM_TICK()
{
    return SYSTEM_TIM_INST->CNT;
}

//! Get microseconds left until next trigger.
size_t GetNextTrigger();

timer_handle_t QueueTimer(void (*cb)(void*), void* obj, size_t intervalUs);

bool AbortTimer(timer_handle_t hnd);

void HWTimerUpdate();
