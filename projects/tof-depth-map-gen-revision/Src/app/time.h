#pragma once
#include <stm32f4xx_hal.h>

/// Accumulated time
extern uint32_t NUM_1KSEC;
#define GLOBAL_TIME_US ( ( (volatile TIM_TypeDef*)htim2.Instance )->CNT )

/// An indicator struct for time in milliseconds and microseconds.
typedef struct {
    uint32_t ms, us;
} timetick_t;

typedef struct {
    uint32_t sec_1k;
    uint32_t usec; //!< usec in range 0 ~ 999,999,999
} ltimetick_t;

static inline int32_t LTime_Diff(ltimetick_t left, ltimetick_t right)
{
    return (int32_t) 1e9 * ( left.sec_1k - right.sec_1k ) + ( (int32_t) left.usec - right.usec );
}

static inline int64_t LTime_LDiff(ltimetick_t left, ltimetick_t right)
{
    return (int64_t) 1e9 * ( left.sec_1k - right.sec_1k ) + ( (int64_t) left.usec - right.usec );
}

static inline ltimetick_t GetLTime()
{
    ltimetick_t ret;
    ret.sec_1k = *(volatile unsigned*) &NUM_1KSEC;
    ret.usec = GLOBAL_TIME_US;
    return ret;
}

/// Get current time in microseconds
static inline timetick_t GetTime()
{
    timetick_t  retval;
    ltimetick_t now = GetLTime();
    retval.ms = now.usec / 1000 + now.sec_1k * (uint32_t) 1e6;
    retval.us = now.usec % 1000;
    return retval;
}