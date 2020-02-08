//! @file       name.x
//! @brief      File brief description
//!
//! @author     Seungwoo Kang (ki6080@gmail.com)
//! @copyright  Copyright (c) 2019. Seungwoo Kang. All rights reserved.
//!
//! @details
//!             File detailed description
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
#include "dist-sensor.h"
#include "hal.h"

/////////////////////////////////////////////////////////////////////////////
// Static decls
static void Test_S2PI();
static void Test_Timer( int argc, char* argv[] );
static void Test_DistSensor( int argc, char* argv[] );
static void Test_Motor( int argc, char* argv[] );

/////////////////////////////////////////////////////////////////////////////
#define SCASE( str )   upp::hash::fnv1a_32_const( str )
#define STRHASH( str ) upp::hash::fnv1a_32( str )

extern "C" bool AppHandler_TestCommand( int argc, char* argv[] )
{
    if ( argc == 0 ) {
        API_Msg(
          "error: this command requires additional argument. \n"
          "Available commands: \n"
          "     dist-sensor\n"
          "     s2pi\n"
          "     motor [args]\n"
          "     timer [num-try] [interval]\n" );
        return true;
    }

    API_Msgf( "info: Test sequence for ::%s:: \n", argv[0] );

    switch ( STRHASH( argv[0] ) ) {
    case SCASE( "s2pi" ):
        Test_S2PI();
        break;
    case SCASE( "timer" ):
        Test_Timer( argc, argv );
        break;
    case SCASE( "dist-sensor" ):
        Test_DistSensor( argc, argv );
        break;
    case SCASE( "motor" ):
        Test_Motor( argc, argv );
        break;
    default:
        API_Msg( "error: Given test argument is not valid. \n" );
        return false;
    }
    return true;
}

/////////////////////////////////////////////////////////////////////////////
// Static impls
void Test_S2PI()
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
    auto cb = []( status_t, void* ) -> status_t {
        API_Msg( "Async function call successful.\n" );
        return 0;
    };

    S2PI_TransferFrameSync( S2PI_SLAVE_ARGUS, tx, rx, sizeof( tx ), cb, NULL );

    for ( size_t i = 0; i < sizeof( tx ); i++ )
        API_Putf( "%x ", tx[i] );
    API_Msg( "\n\t" );
    for ( size_t i = 0; i < sizeof( tx ); i++ )
        API_Putf( "%x ", rx[i] );

    memset( tx + 1, 0, sizeof( tx ) - 1 );
    API_Msg( "\nTesting second pattern:\n\t" );
    S2PI_TransferFrameSync( S2PI_SLAVE_ARGUS, tx, rx, sizeof( tx ), cb, NULL );

    for ( size_t i = 0; i < sizeof( tx ); i++ )
        API_Putf( "%x ", tx[i] );
    API_Msg( "\n\t" );
    for ( size_t i = 0; i < sizeof( tx ); i++ )
        API_Putf( "%x ", rx[i] );

    memset( tx + 1, 0, 16 );
    API_Msg( "\nTesting third pattern:\n\t" );
    S2PI_TransferFrameSync( S2PI_SLAVE_ARGUS, tx, rx, sizeof( tx ), cb, NULL );

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

        API_Msgf(
          "<%3d> %d us (error %d us)\n",
          t.cnt,
          elapsed,
          elapsed - ( t.delay * ( t.cnt + 1 ) ) );

        t.cnt++;
    };

    //! Shuffles timer tick
    //!  If the timers are assigned in random order, the problem is that
    //! the order of nodes is not guaranteed. A careful examination of
    //! the free space list is required.
    int arr[num];
    for ( size_t i = 0; i < num; i++ ) {
        arr[i] = i;
    }

    srand( (uint32_t)API_GetTime_us() );
    for ( size_t i = 0; i < num * 4; i++ ) {
        std::swap( arr[rand() % num], arr[rand() % num] );
    }

    for ( int i = 0; i < num; i++ ) {
        auto correct_delay
          = delay * ( arr[i] + 1 ) - ( API_GetTime_us() - ti.init );
        if ( i < num / 2 ) {
            API_SetTimerFromISR( correct_delay, &ti, timer_cb );
        }
        else {
            API_SetTimer( correct_delay, &ti, timer_cb );
        }
    }
}

void Test_DistSensor( int argc, char* argv[] )
{
    auto retry = 5;
    auto delay = 1000;
    if ( argc >= 3 )
        retry = std::max( retry, (int)strtol( argv[2], NULL, 10 ) );
    if ( argc >= 2 )
        delay = std::max( retry, (int)strtol( argv[1], NULL, 10 ) );

    dist_sens_config_t conf;
    DistSens_GetConfigure( ghDistSens, &conf );
    conf.Delay_us = delay;
    conf.bCloseDistanceMode = true;
    if ( DistSens_Configure( ghDistSens, &conf ) == false ) {
        API_Msg( "error: failed to initialize distance sensor.\n" );
        return;
    }

    API_Msgf( "info: retry count is set to %d\n", retry );

    dist_sens_async_cb_t const cb = []( dist_sens_t h, void*, int result ) {
        if ( result != DIST_SENS_OK ) {
            API_Msgf( "info: failed to capture image with error %d\n", result );
            return;
        }

        q9_22_t val;
        if ( DistSens_GetDistanceFxp( h, &val ) == false ) {
            API_Msg( "warning: failed to get measure result.\n" );
            return;
        }
        API_Msg( "info: succeeded to capture image\n" );
        API_Msgf(
          "info: measured distance is %x.%x \n",
          val >> 22,
          val & ( ( 1 << 22 ) - 1 ) );
    };

    if ( DistSens_MeasureAsync( ghDistSens, retry, NULL, cb ) == false ) {
        API_Msg( "error: failed to start measurement. \n" );
        return;
    }
    API_Msg( "info: measurement triggered. \n" );
}

void Test_Motor( int argc, char* argv[] )
{
    auto const motors = { gMotX, gMotY };

    // Get index
    if ( argc == 1 ) {
        // Report motor status
        int id = 0;
        for ( auto& m : motors ) {
            auto accel = Motor_GetAcceleration( m );
            auto maxs  = Motor_GetMaxSpeed( m );
            auto mins  = Motor_GetMinSpeed( m );
            auto pos   = Motor_Pos( m );

            API_Msgf( // clang-format off
              "info: --- MOTOR <%d> --- \n"
              "         ACCEL   : %u\n"
              "         MAXS    : %u\n"
              "         MINS    : %u\n"
              "         POSITION: %d\n",
              id, accel, maxs, mins, pos ); // clang-format on
            ++id;
        }
        return;
    }
    if ( argc < 3 ) {
        API_Msg( "error: test motor <hw-idx> <delta-steps> " );
        return;
    }

    int hwid  = argv[1][0] == 'x' ? 0 : argv[1][0] == 'y' ? 1 : -1;
    int steps = atoi( argv[2] );

    if ( hwid < 0 || hwid > 1 ) {
        API_Msg( "HWID must be x or y\n" );
        return;
    }

    auto m = motors.begin()[hwid];

    if ( Motor_Stat( m ) != MOTOR_STATE_IDLE ) {
        API_Msg( "error: motor is still running. \n" );
    }

    auto const motor_cb = []( motor_hnd_t m, void* init_pos ) {
        API_Msgf(
          "info: motor movement done. %d ---> %d\n",
          (intptr_t)init_pos,
          Motor_Pos( m ) );
    };

    auto result = Motor_MoveBy( m, steps, motor_cb, (void*)Motor_Pos( m ) );
    if ( result != MOTOR_OK ) {
        API_Msgf(
          "error: motor movement request has failed for code %d\n", result );
        return;
    }

    API_Msg( "info: requested motor movement.\n" );
}
