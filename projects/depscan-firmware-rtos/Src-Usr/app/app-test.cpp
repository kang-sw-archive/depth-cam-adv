#include <FreeRTOS.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uEmbedded-pp/utility.hxx>
#include "../platform/hw.h"
#include "../platform/s2pi.h"
#include "app.h"

/////////////////////////////////////////////////////////////////////////////
// Static decls
static void Test_S2PI();
static void Test_Timer( int argc, char* argv[] );
static void Test_DistSensor( int argc, char* argv[] );

/////////////////////////////////////////////////////////////////////////////
#define CSTRHASH( str ) upp::hash::fnv1a_32_const( str )
#define STRHASH( str )  upp::hash::fnv1a_32( str )

extern "C" bool AppHandler_TestCommand( int argc, char* argv[] )
{
    API_Msgf( "info: Test sequence for ::%s:: \n", argv[0] );

    switch ( STRHASH( argv[0] ) ) {
    case CSTRHASH( "s2pi" ): {
        Test_S2PI();
    } break;

    case CSTRHASH( "timer" ): {
        Test_Timer( argc, argv );
    } break;

    case CSTRHASH( "dist-sensor" ): {
        Test_DistSensor( argc, argv );
    } break;

    default: {
        API_Msg( "error: Given test argument is not valid. \n" );
        API_Msg(
          "Available commands: \n"
          "     dist-sensor\n"
          "     s2pi\n"
          "     timer\n" );
        return false;
    }
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////////
// Static impls
static void Test_S2PI()
{
    // clang-format off
    uint8_t tx[] = {
      0x04,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,
      0x0b,0x0c,0x0d,0x0e,0x0f };
    // clang-format on
    uint8_t rx[sizeof( tx )];
    memset( rx, 0, sizeof( rx ) );
    usec_t us_start = API_GetTime_us();

    API_Msg( "Testing first pattern:\n\t" );
    S2PI_TransferFrameSync(
      S2PI_SLAVE_ARGUS, tx, rx, sizeof( tx ), NULL, NULL );

    for ( size_t i = 0; i < sizeof( tx ); i++ )
        API_Putf( "%x ", tx[i] );
    API_Msg( "\n\t" );
    for ( size_t i = 0; i < sizeof( tx ); i++ )
        API_Putf( "%x ", rx[i] );

    memset( tx + 1, 0, sizeof( tx ) - 1 );
    API_Msg( "\nTesting second pattern:\n\t" );
    S2PI_TransferFrameSync(
      S2PI_SLAVE_ARGUS, tx, rx, sizeof( tx ), NULL, NULL );

    for ( size_t i = 0; i < sizeof( tx ); i++ )
        API_Putf( "%x ", tx[i] );
    API_Msg( "\n\t" );
    for ( size_t i = 0; i < sizeof( tx ); i++ )
        API_Putf( "%x ", rx[i] );

    memset( tx + 1, 0, 16 );
    API_Msg( "\nTesting third pattern:\n\t" );
    S2PI_TransferFrameSync(
      S2PI_SLAVE_ARGUS, tx, rx, sizeof( tx ), NULL, NULL );

    for ( size_t i = 0; i < sizeof( tx ); i++ )
        API_Putf( "%x ", tx[i] );
    API_Msg( "\n\t" );
    for ( size_t i = 0; i < sizeof( tx ); i++ )
        API_Putf( "%x ", rx[i] );

    API_Putf( "\n" );
    API_Msgf( "Elapsed:% d\n", (int)( API_GetTime_us() - us_start ) );
}

void Test_Timer( int argc, char* argv[] )
{
    static struct TI
    {
        uint64_t init;
        int      cnt = 0;
        int      num = 0;
        int      delay;
    } ti;
    int num = 1, delay = 1;

    if ( argc >= 2 )
        num = std::min( NUM_MAX_HWTIMER_NODE, std::max( 1, atoi( argv[1] ) ) );
    if ( argc >= 3 )
        delay = std::max( 100, atoi( argv[2] ) );

    if ( ti.cnt != ti.num ) {
        API_Msg( "Yet timer task is running ... \n" );
        return;
    }
    ti.init  = API_GetTime_us();
    ti.cnt   = 0;
    ti.num   = num;
    ti.delay = delay;

    auto timer_cb = []( void* beg ) {
        auto&    t       = *(TI*)beg;
        uint64_t init    = t.init;
        uint64_t now     = API_GetTime_us();
        int      elapsed = now - init;

        API_Msgf( "<%3d> %d us (error %d us)\n", t.cnt, elapsed,
          elapsed - ( t.delay * ( t.cnt + 1 ) ) );

        t.cnt++;
    };

    for ( int i = 0; i < num; i++ ) {
        auto correct_delay = delay * ( i + 1 ) - ( API_GetTime_us() - ti.init );
        API_SetTimer( correct_delay, &ti, timer_cb );
    }
}
