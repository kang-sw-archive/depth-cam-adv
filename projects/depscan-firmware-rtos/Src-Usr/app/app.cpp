/////////////////////////////////////////////////////////////////////////////
//! Defines fundamental logics for overall system.
//!
//! @author Seungwoo Kang (ki6080@gmail.com)
//! @copyright Copyright (c) 2019. Seungwoo Kang. All rights reserved.
//!
//! @details
#include <FreeRTOS.h>

#include <alloca.h>
#include <list>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <uEmbedded/uassert.h>
#include <vector>
#include "../platform/hw.h"
#include "app.h"
#include "hal.h"
/////////////////////////////////////////////////////////////////////////////
// Memory allocation rebinds
//

void* operator new( size_t size )
{
    return AppMalloc( size );
}

void operator delete( void* p )
{
    AppFree( p );
}

/////////////////////////////////////////////////////////////////////////////
// Statics

/////////////////////////////////////////////////////////////////////////////
// Defines initialize process
//
// - Launches host communication process
// - Launches command queue/procedure process
// - Initializes motor control logic
// - Initializes hi-precision hardware timer
extern "C" void StartDefaultTask( void* nouse_ )
{
    void InitCapture();
    InitHW();
    Internal_InitRW();
    InitCapture();

    // this function never returns.
    AppProc_HostIO( NULL );

    // control never reaches here.
    uassert( false );
    for ( ;; )
        (void)0;
}

/**

@mainpage Depscan firmware
@section Introduction
 This program is firmware that controls two axes of motors and high performance
distance sensors.

@section Interface
 As soon as connect the device with PC, a virtual com port from STM will open.
*/