#include <cmsis_os2.h>
#include <FreeRTOS.h>
#include <list>
#include <stdio.h>
#include <uEmbedded-pp/static_timer_logic.hxx>



upp::static_timer_logic<uint64_t, uint8_t, 100> hw_timer;

extern "C" void CommProc( void )
{
}
