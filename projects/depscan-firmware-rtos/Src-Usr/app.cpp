/////////////////////////////////////////////////////////////////////////////
//! Defines fundamental logics for overall system.
//!
//! @author Seungwoo Kang (ki6080@gmail.com)
//! @copyright Copyright (c) 2019. Seungwoo Kang. All rights reserved.
//!
//! @details
#include "app.h"
#include "arch/hw.h"
#include "rw.h"
#include <FreeRTOS.h>
#include <list>
#include <stdio.h>
#include <string.h>
#include <uEmbedded/uassert.h>
#include <vector>
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
// Globals

/////////////////////////////////////////////////////////////////////////////
// Statics

/////////////////////////////////////////////////////////////////////////////
// Defines initialize process
//
// - Launches host communication process
// - Launches command queue/procedure process
// - Initializes hi-precision hardware timer
// - Initializes motor control logic
extern "C" void StartDefaultTask( void* nouse_ )
{
    InitHW();
    InitRW();

    // Launch hw timer manager

    // Launch Host Communication Process
    AppTask_HostIO( NULL );

    // Never reaches here.
    uassert( false );
}
