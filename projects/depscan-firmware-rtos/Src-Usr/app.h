#pragma once
#include <FreeRTOS.h>

#include "arch/mem.h"
#include "defs.h"
#include <task.h>
#include <uEmbedded/transceiver.h>
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Funcs
_Noreturn void AppProc_HostIO( void* nouse_ );
void           API_SendHostBinary( void const* data, size_t len );
void           API_SendHostString( void const* data, size_t len );
void           API_SendHostRaw( void const* data, size_t len );
bool           AppHandler_CaptureCommand( int argc, char* argv[] );
bool           AppHandler_CaptureBinary( char* data, size_t len );
void           print( char const* fmt, ... ); //!< Print text out to host.
int            putstr( char const* txt );

usec_t         API_GetTime_us();
timer_handle_t API_SetTimer( usec_t delay, void* obj, void ( *cb )( void* ) );
timer_handle_t API_SetTimerFromISR( usec_t delay, void* obj, void ( *cb )( void* ) ); 
void           API_AbortTimer( timer_handle_t h );

#ifdef __cplusplus
}
#endif // __cplusplus
