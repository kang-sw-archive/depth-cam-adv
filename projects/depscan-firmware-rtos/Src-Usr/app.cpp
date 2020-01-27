/////////////////////////////////////////////////////////////////////////////
//! Defines fundamental logics for overall system.
//!
//! @author Seungwoo Kang (ki6080@gmail.com)
//! @copyright Copyright (c) 2019. Seungwoo Kang. All rights reserved.
//!
//! @details
#include "app.h"
#include "rw.h"
#include <FreeRTOS.h>
#include <list>
#include <stdio.h>
#include <string.h>
#include <vector>
/////////////////////////////////////////////////////////////////////////////
// Memory allocation rebinds
//

extern "C" void* malloc( size_t sz )
{
    return pvPortMalloc( sz );
}

extern "C" void free( void* p )
{
    vPortFree( p );
}

void* operator new( size_t size )
{
    return malloc( size );
}

void operator delete( void* p )
{
    free( p );
}

/////////////////////////////////////////////////////////////////////////////
// Thread globals
osThreadId_t gThHostIO;
osThreadId_t gThCmdProc;

/////////////////////////////////////////////////////////////////////////////
// Defines initialize process
//
// - Launches host communication process
// - Launches command queue/procedure process
// - Initializes hi-precision hardware timer
// - Initializes motor control logic
extern "C" void InitProcedure()
{
    InitRW();

    // Launch Host Communication Process
    osThreadAttr_t attr;
    memset( &attr, 0, sizeof( attr ) );
    attr.name       = "HostIO";
    attr.priority   = osPriorityNormal;
    attr.stack_size = 256;
    gThHostIO       = osThreadNew( AppTask_HostIO, NULL, &attr );
}
