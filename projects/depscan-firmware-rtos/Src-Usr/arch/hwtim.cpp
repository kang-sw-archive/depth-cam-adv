#include <FreeRTOS.h>

#include "../defs.h"
#include <stm32f4xx_hal.h>
#include <task.h>
#include <uEmbedded-pp/static_timer_logic.hxx>

/////////////////////////////////////////////////////////////////////////////
// Imports
extern TIM_HandleTypeDef htim2;

/////////////////////////////////////////////////////////////////////////////
// Statics
using timer_t = upp::static_timer_logic<usec_t, uint8_t, NUM_MAX_HWTIMER_NODE>;
static timer_t            s_tim;
static usec_t             s_total_us;
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
#define GET_US() ( htim.Instance->CNT )

/////////////////////////////////////////////////////////////////////////////
// Utility class - timer lock
//

/////////////////////////////////////////////////////////////////////////////
// Defs
extern "C" void HW_TIMER_INIT()
{
    s_tim.tick_function( []() { return API_GetTime_us(); } );

    sTimerTask = xTaskCreateStatic(
        TimerUpdateTask,
        "TIMER",
        sizeof( sTimerStack ) / sizeof( *sTimerStack ),
        NULL,
        configMAX_PRIORITIES - 1,
        sTimerStack,
        &sTimerTaskStaticCb );
}

void TIM2_IRQHandler( void )
{
    // If it's update interrupt, accumulates 1000 second to total time
    if ( __HAL_TIM_GET_FLAG( &htim, TIM_FLAG_UPDATE ) != RESET ) {
        __HAL_TIM_CLEAR_FLAG( &htim, TIM_FLAG_UPDATE );
        s_total_us += (int)1e9;
    }

    // If it's oc interrupt, process hwtimer event and switch to timer task
    if ( __HAL_TIM_GET_FLAG( &htim, TIM_FLAG_CC1 ) != RESET ) {
        __HAL_TIM_CLEAR_FLAG( &htim, TIM_FLAG_CC1 );
        portYIELD_FROM_ISR( pdTRUE );
    }
}

extern "C" usec_t API_GetTime_us()
{
    return s_total_us + GET_US();
}

extern "C" timer_handle_t API_SetTimer( usec_t delay, void* obj, void ( *cb )( void* ) )
{
    uassert( s_tim.capacity() > 0 );
    taskENTER_CRITICAL();
    auto r = s_tim.add( delay, obj, cb );
    // Wake up update task
    xTaskNotifyGive( sTimerTask );
    taskEXIT_CRITICAL();
    return r.id_;
}

extern "C" void API_AbortTimer( timer_handle_t h )
{
    taskENTER_CRITICAL();
    if ( s_tim.remove( { h } ) ) {
        xTaskNotifyGive( sTimerTask );
    }
    taskEXIT_CRITICAL();
}

_Noreturn void TimerUpdateTask( void* nouse__ )
{
    for ( ;; ) {
        ulTaskNotifyTake( pdTRUE, portMAX_DELAY );

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

namespace std {
void __throw_bad_function_call()
{
    uassert( false );
    for ( ;; ) { }
}
} // namespace std