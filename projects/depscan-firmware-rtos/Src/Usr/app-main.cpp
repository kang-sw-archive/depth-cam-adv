#include <FreeRTOS.h>
#include <cmsis_os2.h>
#include <list>
#include <stdio.h>
#include <uEmbedded-pp/static_timer_logic.hxx>

upp::static_timer_logic<uint64_t, uint8_t, 100> hw_timer;

extern "C" void* malloc( size_t sz )
{
    return pvPortMalloc( sz );
}

extern "C" void free( void* p )
{
    vPortFree( p );
}

extern "C" void InitProcedure( void )
{
}
