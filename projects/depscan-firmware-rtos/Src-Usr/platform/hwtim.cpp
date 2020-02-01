#include <FreeRTOS.h>

#include <cmsis_os2.h>
#include <stm32f4xx_hal.h>
#include <task.h>
#include <uEmbedded-pp/static_timer_logic.hxx>
#include "../defs.h"

/////////////////////////////////////////////////////////////////////////////
// Imports
extern TIM_HandleTypeDef htim2;

/////////////////////////////////////////////////////////////////////////////
// Statics
using timer_t = upp::static_timer_logic<usec_t, uint8_t, NUM_MAX_HWTIMER_NODE>;
static timer_t s_tim;
static usec_t  s_total_us = std::numeric_limits<usec_t>::max() - (usec_t)1e9;
static TIM_HandleTypeDef& htim = htim2;

static TaskHandle_t sTimerTask;
static StackType_t  sTimerStack[NUM_TIMER_TASK_STACK_WORDS];
static StaticTask_t sTimerTaskStaticCb;

/////////////////////////////////////////////////////////////////////////////
// Decls
_Noreturn static void TimerUpdateTask( void* nouse__ );
extern "C" usec_t     API_GetTime_us();

/////////////////////////////////////////////////////////////////////////////
// Macro
#define GET_TICK() ( htim.Instance->CNT )

/////////////////////////////////////////////////////////////////////////////
// Utility class - timer lock
//

/////////////////////////////////////////////////////////////////////////////
// Defs
extern "C" void HW_TIMER_INIT()
{
    s_tim.tick_function( []() { return API_GetTime_us(); } );

    sTimerTask = xTaskCreateStatic( TimerUpdateTask, "TIMER",
      sizeof( sTimerStack ) / sizeof( *sTimerStack ), NULL, osPriorityRealtime4,
      sTimerStack, &sTimerTaskStaticCb );

    HAL_TIM_Base_Start_IT( &htim );
    TIM_CCxChannelCmd( htim.Instance, TIM_CHANNEL_1, TIM_CCx_ENABLE );
}

extern "C" void TIM2_IRQHandler( void )
{
    // If it's update interrupt, accumulates 1000 second to total time
    if ( __HAL_TIM_GET_FLAG( &htim, TIM_FLAG_UPDATE ) != RESET ) {
        __HAL_TIM_CLEAR_FLAG( &htim, TIM_FLAG_UPDATE );
        s_total_us += (int)1e9;
    }

    // If it's oc interrupt, process hwtimer event and switch to timer task
    if ( __HAL_TIM_GET_FLAG( &htim, TIM_FLAG_CC1 ) != RESET ) {
        __HAL_TIM_CLEAR_FLAG( &htim, TIM_FLAG_CC1 );
        BaseType_t bHigherTaskPriorityWoken = pdFALSE;
        vTaskNotifyGiveFromISR( sTimerTask, &bHigherTaskPriorityWoken );
        portYIELD_FROM_ISR( bHigherTaskPriorityWoken );
    }
}

extern "C" usec_t API_GetTime_us()
{
    return s_total_us + GET_TICK();
}

extern "C" timer_handle_t
API_SetTimer( usec_t delay, void* obj, void ( *cb )( void* ) )
{
    uassert( s_tim.capacity() > 0 );
    taskENTER_CRITICAL();
    auto r = s_tim.add( std::max( 10ull, delay - 10ull ), obj, cb );
    // Wake update task
    xTaskNotifyGive( sTimerTask );
    taskEXIT_CRITICAL();
    return { r.id_, r.time_ };
}

extern "C" timer_handle_t
API_SetTimerFromISR( usec_t delay, void* obj, void ( *cb )( void* ) )
{
    uassert( s_tim.capacity() > 0 );
    auto r = s_tim.add( std::max( 10ull, delay - 10ull ), obj, cb );

    BaseType_t bHigherTaskPriorityWoken = pdFALSE;
    vTaskNotifyGiveFromISR( sTimerTask, &bHigherTaskPriorityWoken );
    portYIELD_FROM_ISR( bHigherTaskPriorityWoken );
    return { r.id_, r.time_ };
}

extern "C" void API_AbortTimer( timer_handle_t h )
{
    taskENTER_CRITICAL();
    s_tim.remove( { h.data_[0], h.data_[1] } );
    taskEXIT_CRITICAL();
}

_Noreturn void TimerUpdateTask( void* nouse__ )
{
    for ( ;; ) {
        ulTaskNotifyTake( pdTRUE, 100 /* For case if lost ... */ );

        __HAL_TIM_DISABLE_IT( &htim, TIM_FLAG_CC1 );
        auto next = s_tim.update();

        if ( next != (usec_t)-1 ) {
            int delay = next - API_GetTime_us();
            int cnt   = htim.Instance->CNT;
            int arr   = htim.Instance->ARR;

            if ( cnt + delay > arr )
                htim.Instance->CCR1 = ( cnt + delay ) - arr;
            else
                htim.Instance->CCR1 = ( cnt + delay );

            __HAL_TIM_ENABLE_IT( &htim, TIM_FLAG_CC1 );
        }
    }
}

extern "C" bool API_CheckTimer( timer_handle_t h, usec_t* usLeft )
{
    timer_t::desc_type d;
    if ( s_tim.browse( { h.data_[0], h.data_[1] }, d ) == false ) {
        return false;
    }

    if ( usLeft )
        *usLeft = d.trigger_at_ - API_GetTime_us();
    return true;
}

// This code is a dummy function to prevent link errors that occur when using
// the std :: function class.
namespace std {
void __throw_bad_function_call()
{
    uassert( false );
    for ( ;; ) {
    }
}
} // namespace std
