#include "hw-timer.h"
#include <app/program.h>
#include <arch/irq.h>

timer_logic_t   g_timer;
ltimetick_t     pivot_time;
void            HAL_TIM_OC_DelayElapsedCallback( TIM_HandleTypeDef* htim );
extern uint32_t NUM_1KSEC;
#define SYSTIM_CHAN TIM_CHANNEL_1

static inline int32_t from_pvt( ltimetick_t now, ltimetick_t pivot )
{
    return (int32_t)1e9 * ( int32_t )( now.sec_1k - pivot.sec_1k ) + ( int32_t )( now.usec - pivot.usec );
}

static inline int32_t calc_ccr( size_t nextTrig, ltimetick_t now, ltimetick_t pvt )
{
    return (int)nextTrig - from_pvt( now, pvt );
}

void HAL_TIM_PeriodElapsedCallback( TIM_HandleTypeDef* htim )
{
    if ( htim == &SYSTEM_TIM ) {
        NUM_1KSEC++;
    }
}

size_t GetNextTrigger()
{
    if ( g_timer.nodes.size == 0 )
        return -1;

    size_t trig = timer_nextTrigger( &g_timer );
    return calc_ccr( trig, GetLTime(), pivot_time );
}

timer_handle_t QueueTimer( void ( *cb )( void* ), void* obj, size_t intervalUs )
{
    irq_lock();
    // If it's first time, set pivot time ...
    ltimetick_t now = GetLTime();
    if ( g_timer.nodes.size == 0 ) {
        pivot_time = now;
    }

    timer_handle_t retval = timer_add( &g_timer, from_pvt( now, pivot_time ) + intervalUs - 3, cb, obj );
    size_t         trig   = timer_nextTrigger( &g_timer );

    int32_t next = calc_ccr( trig, GetLTime(), pivot_time );
    if ( next < 0 ) {
        // return retval;
        next = 25;
    }
    __HAL_TIM_DISABLE_IT( &SYSTEM_TIM, TIM_IT_CC1 );
    next                  = next + SYSTEM_TIM_TICK();
    SYSTEM_TIM_INST->CCR1 = next > 999999999 ? next - 999999999 : next;
    __HAL_TIM_ENABLE_IT( &SYSTEM_TIM, TIM_IT_CC1 );
    irq_unlock();
    // print("NEXT IS %u for %u interval -- where now is %u\n", SYSTEM_TIM_INST->CCR1, intervalUs, SYSTEM_TIM_TICK());

    return retval;
}

bool AbortTimer( timer_handle_t hnd )
{
    // No other process is required.
    irq_lock();
    bool result = timer_erase( &g_timer, hnd );
    irq_unlock();
    return result;
}

void HWTimerUpdate()
{ 
    for ( ;; ) {
        irq_lock();
        size_t nxt = timer_update( &g_timer, from_pvt( GetLTime(), pivot_time ) );
        irq_unlock();

        if ( g_timer.nodes.size == 0 ) {
            __HAL_TIM_DISABLE_IT( &SYSTEM_TIM, TIM_IT_CC1 );
        }
        else if ( nxt != -1 ) {
            int32_t next = calc_ccr( nxt, GetLTime(), pivot_time );
            if ( next < 0 ) // Repeat process if there's any unprocessed
                continue;

            __HAL_TIM_DISABLE_IT( &SYSTEM_TIM, TIM_IT_CC1 );
            next                  = next + SYSTEM_TIM_TICK();
            SYSTEM_TIM_INST->CCR1 = next > 999999999 ? next - 999999999 : next;
            __HAL_TIM_ENABLE_IT( &SYSTEM_TIM, TIM_IT_CC1 );
        }
        break;
    }
}

// An callback function that called when the nearest timer triggered.
void HAL_TIM_OC_DelayElapsedCallback( TIM_HandleTypeDef* htim )
{
    if ( &SYSTEM_TIM != htim ) {
        return;
    }

    HWTimerUpdate();
    // print("Next trigger is: %d\n", SYSTEM_TIM_INST->CCR1 - SYSTEM_TIM_INST->CNT);
}
