#include "../../app.h"
#include <platform/argus_log.h>
#include <stdarg.h>
#include <stdio.h>

status_t print( const char* fmt, ... )
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

    API_SendHostString( buf, allocsz );
    return STATUS_OK;
}
