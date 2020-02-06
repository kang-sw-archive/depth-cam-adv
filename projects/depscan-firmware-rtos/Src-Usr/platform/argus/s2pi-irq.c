#include <platform/argus_log.h>
#include <main.h>
#include "../../app/app.h"
#include "../s2pi.h"
static s2pi_irq_callback_t g_cbIrq;
static void*               g_cbIrqObj;

static TaskHandle_t s_irq_task;
static StackType_t  s_irq_task_stack[128];
static StaticTask_t s_irq_task_tcb;

void EXTI1_IRQHandler( void )
{
    if ( __HAL_GPIO_EXTI_GET_IT( GPIO_PIN_1 ) != RESET && g_cbIrq ) {
        //~ print( "IRQ function call. \n" );
        //~ g_cbIrq( g_cbIrqObj );
        API_SetTimerFromISR( 0, g_cbIrqObj, g_cbIrq );
    }
    __HAL_GPIO_EXTI_CLEAR_IT( GPIO_PIN_1 );
}

status_t S2PI_SetIrqCallback(
  s2pi_slave_t        slave,
  s2pi_irq_callback_t callback,
  void*               callbackData )
{
    g_cbIrq    = callback;
    g_cbIrqObj = callbackData;
    return STATUS_OK;
}
