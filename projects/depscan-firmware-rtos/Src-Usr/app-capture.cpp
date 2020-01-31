#include <FreeRTOS.h>

#include <argus.h>
#include <uEmbedded-pp/utility.hxx>

#include "app.h"
#include "arch/s2pi.h"

/////////////////////////////////////////////////////////////////////////////
// Statics for capture

#define STRHASH( str ) upp::hash::fnv1a_32_const( #str )

extern "C" bool AppHandler_CaptureCommand( int argc, char* argv[] )
{
    switch ( upp::hash::fnv1a_32( argv[0] ) ) {
    case STRHASH( capture - init ):
        if ( argc < 2 ) {
        }
        auto hnd = Argus_CreateHandle();
        Argus_Init(hnd, S2PI_SLAVE_ARGUS);
        break;
    default:
        return false;
    }
    

    return true;
}

extern "C" bool AppHandler_CaptureBinary( char* data, size_t len )
{
    return true;
}
