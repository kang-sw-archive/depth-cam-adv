#pragma once
#include "arch/mem.h"
#include <FreeRTOS.h>
#include <task.h>
#include <uEmbedded/transceiver.h>
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/////////////////////////////////////////////////////////////////////////////
// Decls
extern transceiver_handle_t gLog;

/////////////////////////////////////////////////////////////////////////////
// Macros
#define OS_MS_TO_TICKS( ms ) ( (ms)*1000u / osKernelGetTickFreq() )

/////////////////////////////////////////////////////////////////////////////
// Funcs
void AppTask_HostIO( void* nouse_ );
void AppTask_CmdProc( void* nouse_ );

#ifdef __cplusplus
}
#endif // __cplusplus
