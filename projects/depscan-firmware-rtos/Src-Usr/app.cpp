/////////////////////////////////////////////////////////////////////////////
//! Defines fundamental logics for overall system.
//!
//! @author Seungwoo Kang (ki6080@gmail.com)
//! @copyright Copyright (c) 2019. Seungwoo Kang. All rights reserved.
//!
//! @details
#include <FreeRTOS.h>

#include "app.h"
#include "arch/hw.h"
#include "rw.h"
#include <alloca.h>
#include <list>
#include <stdarg.h>
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
extern "C" void API_Logf( char const* fmt, ... )
{
    va_list vp;
    va_list vp2;
    va_start( vp, fmt );
    size_t allocsz = vsnprintf( NULL, 0, fmt, vp ) + 1;
    va_end( vp );

    va_copy( vp2, vp );
    char* buf = (char*)alloca( allocsz );
    vsprintf( buf, fmt, vp2 );
    va_end( vp2 );

    API_Log( buf );
}

extern "C" int API_Log( char const* txt )
{
    auto t = API_GetTime_us();
    char buf[16];
    sprintf( buf, "[%6u.%06u]", ( uint32_t )( t / 1000000u ), ( uint32_t )( t % 1000000u ) );
    API_SendHostString( buf, sizeof( buf ) );
    API_SendHostString( txt, strlen( txt ) + 1 );
    return 0;
}

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

    // launch hw timer manager
    //! @todo. implement this

    // this function never returns.
    AppProc_HostIO( NULL );

    // control never reaches here.
    uassert( false );
}
