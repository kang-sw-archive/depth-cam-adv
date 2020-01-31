#include <FreeRTOS.h>
#include <platform/argus_irq.h>

void IRQ_LOCK( void )
{
    vPortEnterCritical();
}

void IRQ_UNLOCK( void )
{
    vPortExitCritical();
}
