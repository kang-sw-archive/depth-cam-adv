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
using timer_t = upp::static_timer_logic<nano_sec_t, uint8_t, NUM_MAX_HWTIMER_NODE>;
static timer_t            s_tim;
static nano_sec_t         s_total_ns;
static TIM_HandleTypeDef& htim = htim2;

/////////////////////////////////////////////////////////////////////////////
// Decls
nano_sec_t     API_GetTime_ns();
timer_handle_t API_SetTimer( nano_sec_t delay, void* obj, void ( *cb )( void* ) );
void           API_AbortTimer( timer_handle_t h );

/////////////////////////////////////////////////////////////////////////////
// Macro
#define GET_NS() ( htim.Instance->CNT * (uint64_t)1e9 / SystemCoreClock )

/////////////////////////////////////////////////////////////////////////////
// Defs
void HW_TIMER_INIT()
{
    s_tim.tick_function( []() { return API_GetTime_ns(); } );
}

void TIM2_IRQHandler( void )
{
    // If it's update interrupt, accumulates 1 second to total time
    if ( __HAL_TIM_GET_FLAG( &htim, TIM_FLAG_UPDATE ) != RESET ) {
        s_total_ns += (int)1e9;
        __HAL_TIM_CLEAR_FLAG( &htim, TIM_FLAG_UPDATE );
    }

    // If it's oc interrupt, process hwtimer event and
    if ( __HAL_TIM_GET_FLAG( &htim, TIM_FLAG_CC1 ) != RESET ) {
        __HAL_TIM_CLEAR_FLAG( &htim, TIM_FLAG_CC1 );
    }
}

nano_sec_t API_GetTime_ns()
{
    return s_total_ns + GET_NS();
}

timer_handle_t API_SetTimer( nano_sec_t delay, void* obj, void ( *cb )( void* ) )
{
    return timer_handle_t();
}

void API_AbortTimer( timer_handle_t h )
{
}
