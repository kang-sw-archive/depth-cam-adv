#include <FreeRTOS.h>

#include <string.h>
#include <uEmbedded-pp/utility.hxx>
#include "../protocol/protocol-s.h"
#include "app.h"
#include "capture.h"
#include "dist-sensor.h"
#include "hal.h"

/////////////////////////////////////////////////////////////////////////////
// Globals
capture_t gCapture = { .AnglePerStep = { .x = 1.8f / 32.f, .y = 1.8f / 32.f } };

/////////////////////////////////////////////////////////////////////////////
// Aliasing for easy use
capture_t& cc = gCapture;

/////////////////////////////////////////////////////////////////////////////
// Capture Handler
#define CSTRHASH( str ) upp::hash::fnv1a_32_const( str )
#define STRHASH( str )  upp::hash::fnv1a_32( str )
static void HandleConfig( int argc, char* argv[] );

extern "C" bool AppHandler_CaptureCommand( int argc, char* argv[] )
{
    if ( argc == 0 ) {
        API_Msg( "error: this command requires addtional argument.\n" );
        return true;
    }

    switch ( upp::hash::fnv1a_32( argv[0] ) ) {
    case CSTRHASH( "init-sensor" ):
    {
        if ( DistSens_Configure( ghDistSens, nullptr ) == false ) {
            API_Msg( "error: failed to initialize distance sensor \n" );
        }
    } break;

    case CSTRHASH( "report" ):
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
    } break;

    case CSTRHASH( "config" ):
    {
        HandleConfig( argc - 1, argv + 1 );
    } break;

    default:
    case CSTRHASH( "help" ):
    {
        API_Msg(
          " usage: capture <command> [args...] \n"
          " command list: \n"
          "   report              Update report\n"
          "   start               Request start scan, or pause, or resume.\n"
          "   stop                Request stop\n"
          "   motor-move [x [y]]  Request motor movement. Units are in step.\n"
          "   config [args]       Configure arguments; more: try type "
          "\"capture config\"\n"
          "   help                \n" );
    } break;
    }

    return true;
}

extern "C" bool AppHandler_CaptureBinary( char* data, size_t len )
{
    return true;
}
