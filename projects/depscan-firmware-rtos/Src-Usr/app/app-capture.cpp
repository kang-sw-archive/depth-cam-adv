#include <FreeRTOS.h>

#include <argus.h>
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

    case CSTRHASH( "pattern-test" ): {
        uint8_t tx[] = { 0x04, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
        uint8_t rx[sizeof( tx )];
        memset( rx, 0, sizeof( rx ) );
        usec_t us_start = API_GetTime_us();

        API_Log( "Testing first pattern:\n\t" );
        S2PI_TransferFrameSync( S2PI_SLAVE_ARGUS, tx, rx, sizeof( tx ), NULL, NULL );

        for ( size_t i = 0; i < sizeof( tx ); i++ )
            API_Putf( "%x ", tx[i] );
        API_Log( "\n\t" );
        for ( size_t i = 0; i < sizeof( tx ); i++ )
            API_Putf( "%x ", rx[i] );

        memset( tx + 1, 0, sizeof( tx ) - 1 );
        API_Log( "\nTesting second pattern:\n\t" );
        S2PI_TransferFrameSync( S2PI_SLAVE_ARGUS, tx, rx, sizeof( tx ), NULL, NULL );

        for ( size_t i = 0; i < sizeof( tx ); i++ )
            API_Putf( "%x ", tx[i] );
        API_Log( "\n\t" );
        for ( size_t i = 0; i < sizeof( tx ); i++ )
            API_Putf( "%x ", rx[i] );

        memset( tx + 1, 0, 16 );
        API_Log( "\nTesting third pattern:\n\t" );
        S2PI_TransferFrameSync( S2PI_SLAVE_ARGUS, tx, rx, sizeof( tx ), NULL, NULL );

        for ( size_t i = 0; i < sizeof( tx ); i++ )
            API_Putf( "%x ", tx[i] );
        API_Log( "\n\t" );
        for ( size_t i = 0; i < sizeof( tx ); i++ )
            API_Putf( "%x ", rx[i] );

        API_Putf( "\n Elapsed: %d\n", (int)( API_GetTime_us() - us_start ) );
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
