#include <platform/argus_log.h>
#include <stm32f4xx_hal.h>
#include "../s2pi.h"
static s2pi_irq_callback_t g_cbIrq;
static void*               g_cbIrqObj;

void EXTI1_IRQHandler( void )
{
    __HAL_GPIO_EXTI_CLEAR_IT( GPIO_PIN_1 );
    if ( g_cbIrq ) {
        print( "IRQ function call. \n" );
        g_cbIrq( g_cbIrqObj );
    }
}

status_t S2PI_SetIrqCallback( s2pi_slave_t        slave,
                              s2pi_irq_callback_t callback,
                              void*               callbackData )
{
    g_cbIrq    = callback;
    g_cbIrqObj = callbackData;
    return STATUS_OK;
}
