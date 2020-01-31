#pragma once
#include <FreeRTOS.h>

#include <stdbool.h>
#include <task.h>
#include <uEmbedded/transceiver.h>
#include "../defs.h"
#include "../platform/mem.h"
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// APIs
// Default functionality

// File xfer funtions
void API_SendHostBinary( void const* data, size_t len );
void API_SendHostBinaries( void const* const data[], size_t const len[], size_t cnt );
void API_SendHostString( void const* data, size_t len );
void API_SendHostRaw( void const* data, size_t len ); //< Does not wrap data with header.

void API_Logf( char const* fmt, ... ); //!< Print text out to host.
int  API_Log( char const* txt );

// Assign data export
void API_ExportBin( uint32_t id, void const* mem, size_t len, char const* name );
void API_RemoveExport( uint32_t id, void const* ptr );

// Timer functionality
usec_t         API_GetTime_us();
timer_handle_t API_SetTimer( usec_t delay, void* obj, void ( *cb )( void* ) );
timer_handle_t API_SetTimerFromISR( usec_t delay, void* obj, void ( *cb )( void* ) );
void           API_AbortTimer( timer_handle_t h );
bool           API_CheckTimer( timer_handle_t h, usec_t* usLeft );

/////////////////////////////////////////////////////////////////////////////
// Procedures
_Noreturn void AppProc_HostIO( void* nouse_ );
bool           AppHandler_CaptureCommand( int argc, char* argv[] );
bool           AppHandler_CaptureBinary( char* data, size_t len );

#ifdef __cplusplus
}
#endif // __cplusplus
