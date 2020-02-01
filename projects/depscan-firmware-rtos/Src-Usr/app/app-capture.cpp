#include <FreeRTOS.h>

#include <string.h>
#include <uEmbedded-pp/utility.hxx>
#include "../platform/s2pi.h"
#include "app.h"
#include "capture.h"

/////////////////////////////////////////////////////////////////////////////
// Globals
capture_t gCapture;

/////////////////////////////////////////////////////////////////////////////
// Capture Handler
#define CSTRHASH( str ) upp::hash::fnv1a_32_const( str )
#define STRHASH( str )  upp::hash::fnv1a_32( str )

extern "C" bool AppHandler_CaptureCommand( int argc, char* argv[] )
{
    switch ( upp::hash::fnv1a_32( argv[0] ) ) {
    case CSTRHASH( "init" ): {
        if ( argc < 2 ) {
            API_Log(
                "Initialization must specify capture options. usage: \n"
                "  capture-init <mode>\n"
                "       mode: scan  - Scanning mode. \n"
                "             point - Point specification mode. \n" );
            break;
        }

        auto arg = STRHASH( argv[1] );
        if ( arg == CSTRHASH( "scan" ) ) {
        }
        else if ( arg == CSTRHASH( "point" ) ) {
        }
        else {
        }
    } break;

    case CSTRHASH( "close" ): {
    }
    default:
        return false;
    }

    return true;
}

extern "C" bool AppHandler_CaptureBinary( char* data, size_t len )
{
    return true;
}
