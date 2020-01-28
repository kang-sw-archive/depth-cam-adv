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
void AppTask_HostIO( void* nouse_ );
void API_SendHostBinary( void const* data, size_t len );
void API_SendHostString( void const* data, size_t len );

#ifdef __cplusplus
}
#endif // __cplusplus
