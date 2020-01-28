#pragma once
#include "arch/mem.h"
#include <FreeRTOS.h>
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
void           print( char const* fmt, ... ); //!< Print text out to host.
int            puts( char const* txt ); 

#ifdef __cplusplus
}
#endif // __cplusplus
