//! @file       app-capture.cpp
//! @brief      Primary capture sessions
//!
//! @author     Seungwoo Kang (ki6080@gmail.com)
//! @copyright  Copyright (c) 2019. Seungwoo Kang. All rights reserved.
//!
//! @details
//!             File detailed description
//! @todo       Implement line scanning session
//! @todo       Implement point capture session
//!             Especially, binary command handler for large amount of requests
#include <FreeRTOS.h>

#include <optional>
#include <string.h>
#include <uEmbedded-pp/utility.hxx>
#include "../protocol/protocol-s.h"
#include "app.h"
#include "dist-sensor.h"
#include "hal.h"
#include "internal-capture.h"

/////////////////////////////////////////////////////////////////////////////
// Globals
capture_t gCapture = { .AnglePerStep = { .x = 1.8f / 32.f, .y = 1.8f / 32.f } };
char      Capture_Buffer[CAPTURER_BUFFER_SIZE];

/////////////////////////////////////////////////////////////////////////////
// Aliasing for easy use
capture_t& cc = gCapture;

/////////////////////////////////////////////////////////////////////////////
// Initialize capturer
// Assignment of some export variables ...
extern "C" void InitCapture()
{
    //! @todo Export variables for monitoring from host
}

/////////////////////////////////////////////////////////////////////////////
// Capture Handler
#define SCASE( str )   upp::hash::fnv1a_32_const( str )
#define STRHASH( str ) upp::hash::fnv1a_32( str )
static void HandleConfig( int argc, char* argv[] );

extern "C" bool AppHandler_CaptureCommand( int argc, char* argv[] )
{
    // String command handler

    if ( argc == 0 )
    {
        API_Msg( "error: this command requires additional argument.\n" );
        return true;
    }

    switch ( upp::hash::fnv1a_32( argv[0] ) )
    {
    case SCASE( "init-sensor" ):
    {
        if ( DistSens_Configure( ghDistSens, nullptr ) == false )
        {
            API_Msg( "error: failed to initialize distance sensor \n" );
        }
    }
    break;

    case SCASE( "report" ):
    {
        dist_sens_config_t opt;
        DistSens_GetConfigure( ghDistSens, &opt );

        FDeviceStat r;
        r.bIsIdle              = cc.CaptureProcess != NULL;
        r.bIsPaused            = cc.bPaused;
        r.bIsIdle              = cc.CaptureProcess == NULL;
        r.bIsPrecisionMode     = opt.bCloseDistanceMode;
        r.bIsSensorInitialized = ghDistSens != NULL;
        r.CurMotorStepX        = cc.Scan_Pos.x;
        r.CurMotorStepY        = cc.Scan_Pos.y;
        r.DegreePerStepX       = cc.AnglePerStep.x;
        r.DegreePerStepY       = cc.AnglePerStep.y;
        r.DelayPerCapture      = opt.Delay_us;
        r.OfstX                = cc.Scan_CaptureOfst.x;
        r.OfstY                = cc.Scan_CaptureOfst.y;
        r.SizeX                = cc.Scan_Resolution.x;
        r.SizeY                = cc.Scan_Resolution.y;
        r.StepPerPxlX          = cc.Scan_StepPerPxl.x;
        r.StepPerPxlY          = cc.Scan_StepPerPxl.y;
        r.TimeAfterLaunch_us   = API_GetTime_us();

        SCANNER_COMMAND_TYPE cmd = ECommand::RSP_STAT_REPORT;

        void* const  dat[] = { &cmd, &r };
        size_t const len[] = { sizeof( cmd ), sizeof( r ) };
        API_SendHostBinaries( dat, len, 2 );
    }
    break;

    case SCASE( "config" ):
    {
        HandleConfig( argc - 1, argv + 1 );
    }
    break;

    case SCASE( "help" ):
    {
        API_Msg(
          " usage: capture <command> [args...] \n"
          " command list: \n"
          "   report              Update report\n"
          "   scan-start          Request start scan, or pause, or resume.\n"
          "   point-start         Request point mode to start. \n"
          "   pause               Request pause current session \n"
          "   stop                Request stop for any session\n"
          "   motor-move [x [y]]  Request motor movement. Units are in step.\n"
          "   config [args]       Configure arguments; more: try type "
          "\"capture config\"\n"
          "   help                \n" );
    }
    break;
    default:
    {
        API_Msgf( "warning: unknown capture command [%s]\n", argv[0] );
    }
    break;
    }

    return true;
}

extern "C" bool AppHandler_CaptureBinary( char* data, size_t len )
{
    // Binary command handler
    return true;
}

/////////////////////////////////////////////////////////////////////////////
void HandleConfig( int argc, char* argv[] )
{
    if ( Capture_IsRunning() )
    {
        API_Msg(
          "error: configuration is not allowed during capture session.\n" );
        return;
    }

    if ( argc == 0 )
    {
        API_Msg(
          "info: Config option is not specified.\n"
          "  usage: <operation> [value1 [value 2 [...]]]\n"
          "        Put -1 not to modify value.\n"
          "     Options\n"
          "        offset          Capture begin offset in motor steps\n"
          "        resolution      Output image size\n"
          "        step-per-pixel  Required motor steps per pixel capture\n"
          "        delay           Capture delay in microseconds. Will be "
          "applied on the next initialization.\n"
          "        angle-per-step  Set angle per steps. Use reinterpret cast "
          "of float value. \n"
          "        precision       Set precision mode. 0 to far, non-zero to "
          "close. \n"
          "        motor-max-clk   Motor maximum clock speed.\n"
          "        motor-accel     Motor acceleration speed. \n" );
        return;
    }
    std::optional<int> xv, yv;
    char*              det;
    int                tmp;

    if ( argc >= 2 && ( tmp = strtol( argv[1], &det, 10 ), argv[1] != det ) )
    {
        xv = tmp;
    }
    if ( argc >= 3 && ( tmp = strtol( argv[2], &det, 10 ), argv[2] != det ) )
    {
        yv = tmp;
    }

    if ( !xv && !yv )
    {
        API_Msg( "error: invalid configuration arguments\n" );
        return;
    }

    switch ( STRHASH( argv[0] ) )
    {
    case SCASE( "offset" ):
        if ( xv )
            cc.Scan_CaptureOfst.x = *xv;
        if ( yv )
            cc.Scan_CaptureOfst.y = *yv;

        API_Msgf(
          "info: offset is set as %d, %d\n",
          cc.Scan_CaptureOfst.x,
          cc.Scan_CaptureOfst.y );
        break;

    case SCASE( "resolution" ):
        if ( xv > 0 )
            cc.Scan_Resolution.x = *xv;
        if ( yv > 0 )
            cc.Scan_Resolution.y = *yv;
        API_Msgf(
          "info: resolution is set as %d, %d\n",
          cc.Scan_Resolution.x,
          cc.Scan_Resolution.y );
        break;

    case SCASE( "step-per-pixel" ):
        if ( xv > 0 )
            cc.Scan_StepPerPxl.x = *xv;
        if ( yv > 0 )
            cc.Scan_StepPerPxl.y = *yv;
        API_Msgf(
          "info: steps per pixel is set as %d, %d\n",
          cc.Scan_StepPerPxl.x,
          cc.Scan_StepPerPxl.y );
        break;

    case SCASE( "delay" ):
    {
        if ( xv > 0 == false )
        {
            API_Msg(
              "error: invalid argument for delay. value must be positive "
              "integer.\n" );
            break;
        }

        dist_sens_config_t conf;
        DistSens_GetConfigure( ghDistSens, &conf );
        conf.Delay_us = *xv;

        if ( DistSens_Configure( ghDistSens, &conf ) == false )
        {
            API_Msg( "warning: failed to apply delay configuration\n" );
        }
        else
        {
            API_Msgf( "info: delay is %u us\n", conf.Delay_us );
        }
    }
    break;

    case SCASE( "angle-per-step" ):
    {
        // Force convert to float
        union
        {
            float f;
            int   s__;
        } x, y;
        x.s__ = 0, y.s__ = 0;

        if ( xv > 0 )
            x.s__ = *xv;
        if ( yv > 0 )
            y.s__ = *yv;

        if ( x.f > 0.f && x.f < 1.0e6f )
            cc.AnglePerStep.x = x.f;
        if ( y.f > 0.f && y.f < 1.0e6f )
            cc.AnglePerStep.y = y.f;

        API_Msgf(
          "info: angle per step set ... %d, %d [udeg]\n",
          (int)( cc.AnglePerStep.x * 1e6f ),
          (int)( cc.AnglePerStep.y * 1e6f ) );
    }
    break;

    case SCASE( "precision" ):
    {
        dist_sens_config_t conf;
        DistSens_GetConfigure( ghDistSens, &conf );
        conf.bCloseDistanceMode = xv != 0;
        if ( DistSens_Configure( ghDistSens, &conf ) == false )
        {
            API_Msg( "warning: sensor precision mode configuration failed.\n" );
        }
        else
        {
            API_Msgf(
              "info: current configuration: %s\n",
              conf.bCloseDistanceMode ? "Near" : "Far" );
        }
    }
    break;

    case SCASE( "motor-max-clk" ):
        if ( xv > 0 )
        {
            Motor_SetMaxSpeed( gMotX, *xv );
        }
        if ( yv > 0 )
        {
            Motor_SetMaxSpeed( gMotY, *yv );
        }
        break;

    case SCASE( "motor-accel" ):
        if ( xv > 0 )
        {
            Motor_SetAcceleration( gMotX, *xv );
        }
        if ( yv > 0 )
        {
            Motor_SetAcceleration( gMotY, *yv );
        }
        break;

    default:
        API_Msgf( "warning: unknown configuration property [%s]\n", argv[0] );
        break;
    }
}
