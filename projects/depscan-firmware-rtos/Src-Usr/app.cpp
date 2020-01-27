/////////////////////////////////////////////////////////////////////////////
//! Defines fundamental logics for overall system.
//!
//! @author Seungwoo Kang (ki6080@gmail.com)
//! @copyright Copyright (c) 2019. Seungwoo Kang. All rights reserved.
//!
//! @details
#include "rw.h"
#include <FreeRTOS.h>
#include <cmsis_os2.h>
#include <list>
#include <stdio.h>
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
// Static decls
static void InitCmdProc();

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
}
