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

#include <array>
#include <optional>
#include <string.h>
#include <uEmbedded-pp/utility.hxx>
#include <uEmbedded/uassert.h>
#include "../protocol/protocol-s.h"
#include "app.h"
#include "dist-sensor.h"
#include "hal.h"
#include "internal-capture.h"

/////////////////////////////////////////////////////////////////////////////
// Constants
enum : size_t
{
    POINT_TOTAL_BUF_SZ     = sizeof( Capture_Buffer ),
    POINT_NUM_RDQUEUE_ELEM = POINT_TOTAL_BUF_SZ / sizeof( FPointReq ),
};

/////////////////////////////////////////////////////////////////////////////
// Globals
capture_t gCapture = {
  .AnglePerStep        = { .x = 1.8f / 32.f, .y = 1.8f / 32.f },
  .Scan_Resolution     = { .x = 10, .y = 10 },
  .Scan_StepPerPxl     = { .x = 10, .y = 1 },
  .Point_NumMaxRequest = POINT_NUM_RDQUEUE_ELEM };
char Capture_Buffer[CAPTURER_BUFFER_SIZE];
/////////////////////////////////////////////////////////////////////////////
// Aliasing for easy use
capture_t& cc = gCapture;

/////////////////////////////////////////////////////////////////////////////
// Initialize capturer
// Assignment of some export variables ...
extern "C" void InitCapture()
{
    //! @todo Export variables for monitoring from host
#define EXPORT( NAME, VAR )                                                    \
    API_ExportBin( upp::hash::fnv1a_32_const( NAME ), &VAR, sizeof( VAR ) )

    EXPORT( "point-queue-max", cc.Point_NumMaxRequest );
    EXPORT( "point-queue-cur", cc.Point_NumPendingRequest );
#undef EXPORT
}

/////////////////////////////////////////////////////////////////////////////
// Capture Handler
#define SCASE( str )   upp::hash::fnv1a_32_const( str )
#define STRHASH( str ) upp::hash::fnv1a_32( str )
static void HandleConfig( int argc, char* argv[] );
static void Task_Scan( void* nouse_ );
static void Task_Point( void* nouse_ );
static void Task_Enqeuue( int argc, char* argv[] );
static void InitCaptureTask( void ( *cb )( void* ) );

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
        r.bIsPaused                 = cc.bPaused;
        r.bIsIdle                   = cc.CaptureTask == NULL;
        r.bIsPrecisionMode          = opt.bCloseDistanceMode;
        r.bIsSensorInitialized      = ghDistSens != NULL;
        r.CurMotorStepX             = Motor_GetPos( gMotX );
        r.CurMotorStepY             = Motor_GetPos( gMotY );
        r.DegreePerStepX            = cc.AnglePerStep.x;
        r.DegreePerStepY            = cc.AnglePerStep.y;
        r.DelayPerCapture           = opt.Delay_us;
        r.OfstX                     = cc.Scan_CaptureOfst.x;
        r.OfstY                     = cc.Scan_CaptureOfst.y;
        r.SizeX                     = cc.Scan_Resolution.x;
        r.SizeY                     = cc.Scan_Resolution.y;
        r.StepPerPxlX               = cc.Scan_StepPerPxl.x;
        r.StepPerPxlY               = cc.Scan_StepPerPxl.y;
        r.NumMaxPointRequest        = POINT_NUM_RDQUEUE_ELEM;
        r.NumProcessingPointRequest = cc.Point_NumPendingRequest;
        r.TimeAfterLaunch_us
          = API_GetTime_us() - !r.bIsIdle * cc.TimeProcessBegin;

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

    case SCASE( "motor-move" ):
    {
        // This request does not affect to motor's internal position value
        char* det;
        int   tmp;

        if ( argc >= 2 )
            if ( tmp = strtol( argv[1], &det, 10 ), argv[1] != det )
            {
                Motor_SetPos( gMotX, Motor_GetPos( gMotX ) - tmp );
                Motor_MoveBy( gMotX, tmp, NULL, NULL );
                API_Msgf( "info: queue x motor movement %d \n", tmp );
            }
        if ( argc >= 3 )
            if ( tmp = strtol( argv[2], &det, 10 ), argv[2] != det )
            {
                Motor_SetPos( gMotY, Motor_GetPos( gMotY ) - tmp );
                Motor_MoveBy( gMotY, tmp, NULL, NULL );
                API_Msgf( "info: queue x motor movement %d \n", tmp );
            }
    }
    break;

    case SCASE( "scan-start" ):
    {
        InitCaptureTask( Task_Scan );
    }
    break;

    case SCASE( "point-start" ):
    {
        InitCaptureTask( Task_Point );
    }
    break;

    case SCASE( "point-queue" ):
    {
        Task_Enqeuue( argc - 1, argv + 1 );
    }
    break;

    case SCASE( "stop" ):
    {
        if ( cc.CaptureTask == NULL )
        {
            API_Msg( "warning: process is already in idle state.\n" );
            break;
        }

        API_Msg( "info: requesting stop ... \n" );
        cc.bPendingStop = true;
    }
    break;

    case SCASE( "pause" ):
    {
        if ( cc.CaptureTask == NULL )
        {
            API_Msg( "warning: process is already in idle state.\n" );
            break;
        }

        API_Msgf(
          "info: requesting %s ... \n", cc.bPaused ? "resume" : "pause" );
        cc.bPaused = !cc.bPaused;
    }

    case SCASE( "help" ):
    {
        API_Msg(
          " usage: capture <command> [args...] \n"
          " command list: \n"
          "   report              Update report\n"
          "   scan-start          Request to start scan.\n"
          "   point-start         Request to point mode to start. \n"
          "   point-queue         Request to enqueue new point capture \n"
          "   pause               Request to pause current session \n"
          "   stop                Request to stop for any session\n"
          "   motor-move [x [y]]  Request to motor movement. Units are in "
          "step.\n"
          "   motor-reset         Reset motor's current position \n"
          "   config [args]       Configure arguments; more: try type "
          "\"capture config\"\n"
          "   help                \n" );
    }
    break;

    case SCASE( "version" ):
    {
        char VersionString[256];
        DistSens_GetVersion( ghDistSens, VersionString, sizeof VersionString );
        API_Msg( VersionString );
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
        if ( ( xv > 0 ) == false )
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
            API_Msgf( "info: Motor speed X has set to %d\n", *xv );
        }
        if ( yv > 0 )
        {
            Motor_SetMaxSpeed( gMotY, *yv );
            API_Msgf( "info: Motor speed Y has set to %d\n", *yv );
        }
        break;

    case SCASE( "motor-accel" ):
        if ( xv > 0 )
        {
            Motor_SetAcceleration( gMotX, *xv );
            API_Msgf( "info: Motor acceleration X has set to %d\n", *xv );
        }
        if ( yv > 0 )
        {
            Motor_SetAcceleration( gMotY, *yv );
            API_Msgf( "info: Motor acceleration Y has set to %d\n", *yv );
        }
        break;

    default:
        API_Msgf( "warning: unknown configuration property [%s]\n", argv[0] );
        break;
    }
}

void InitCaptureTask( void ( *cb )( void* ) )
{
    if ( cc.CaptureTask != NULL )
    {
        API_Msg( "error: capturing process is already in progress!\n" );
        return;
    }

    auto res = xTaskCreate(
      cb,
      "Scan",
      CAPTURE_TASK_STACK_DEPTH,
      NULL,
      TaskPriorityNormal,
      &cc.CaptureTask );

    cc.bPaused          = false;
    cc.bPendingStop     = false;
    cc.TimeProcessBegin = API_GetTime_us();

    if ( res == pdFALSE || cc.CaptureTask == NULL )
    {
        cc.CaptureTask = NULL;
        API_Msg( "fatal: failed to initialize capturing process\n" );
    }
    else
    {
        API_Msg( "info: scanning process is initialized.\n" );
    }
}

/////////////////////////////////////////////////////////////////////////////
// Helpers for internal usage
static void wait_motor()
{
    while ( Motor_Stat( gMotX ) != MOTOR_STATE_IDLE
            || Motor_Stat( gMotY ) != MOTOR_STATE_IDLE )
    {
        taskYIELD();
    }
}

static std::optional<FPxlData>
TryMeasureDistance( int NumRetry, status_t* outResult = nullptr )
{
    status_t placeholder__; // Prevents segmentation fault
    if ( outResult == nullptr )
    {
        outResult = &placeholder__;
    }

    for ( ; ( *outResult = DistSens_MeasureSync( ghDistSens, 5 ) ) < 0; )
    {
        if ( NumRetry-- == 0 )
        {
            API_Msg(
              "error: all measurement retries exhausted. aborting... \n" );
            return {};
        }
        API_Msgf(
          "warning: measurement failed for code %d. Retry after 300ms ... "
          "retries left "
          "%d\n",
          *outResult,
          NumRetry );
        vTaskDelay( pdMS_TO_TICKS( 300 ) );
        DistSens_Configure( ghDistSens, NULL );
    }

    FPxlData data;
    bool     bMeasureRes;
    bMeasureRes = DistSens_GetDistanceFxp( ghDistSens, &data.Distance )
                  && DistSens_GetAmpFxp( ghDistSens, &data.AMP );

    // In this statement, data acquisition must succeed.
    uassert( bMeasureRes );
    return data;
}

/////////////////////////////////////////////////////////////////////////////
// Scanning process that performs pixel-by-pixel capture
static void Task_Scan( void* nouse_ )
{
    usec_t BeginTime = API_GetTime_us();
    // Clear all status variables
    auto&      pos        = cc.Scan_Pos;
    auto const resolution = cc.Scan_Resolution;
    auto const ofst       = cc.Scan_CaptureOfst;
    auto const steps      = cc.Scan_StepPerPxl;
    pos.x = 0, pos.y = 0;
    FLineDesc     desc;
    constexpr int NumMaxBufferedPxl
      = std::min( sizeof( Capture_Buffer ) / sizeof( FPxlData ), size_t( 24 ) );
    auto& pixels = reinterpret_cast<std::array<FPxlData, NumMaxBufferedPxl>&>(
      Capture_Buffer );

    desc.LineIdx = 0;
    desc.OfstX   = 0;
    desc.NumPxls = 0;

    enum dir_t
    {
        DIR_FWD = 1,
        DIR_BWD = -1
    };
    dir_t dir = DIR_FWD;

    // Relocate motor to its origin position in sync
    wait_motor();
    Motor_MoveTo( gMotX, cc.Scan_CaptureOfst.x, NULL, NULL );
    Motor_MoveTo( gMotY, cc.Scan_CaptureOfst.y, NULL, NULL );
    wait_motor();

    // Discard first few samples
    API_Msg( "info: discarding first few samples ... \n" );
    for ( size_t i = 0; i < CAPTURE_NUM_INITIAL_DISCARDS; i++ )
    {
        if ( !TryMeasureDistance( CAPTURE_NUM_MEASUREMENT_RETRY ) )
        {
            API_Msg( "error: sensor is in incorrect state. aborting ...\n" );
            goto ABORT;
        }
    }

    for ( ; pos.y < resolution.y; )
    {
        // 0. Check for escape condition
        if ( cc.bPendingStop )
        {
            break;
        }
        if ( cc.bPaused )
        {
            vTaskDelay( pdMS_TO_TICKS( 10 ) );
            continue;
        }

        // 1. Captures distance
        FPxlData data;
        for ( size_t i = 0; i < CAPTURE_NUM_MEASUREMENT_RETRY; ++i )
        {
            status_t result;
            if ( auto measurement
                 = TryMeasureDistance( CAPTURE_NUM_MEASUREMENT_RETRY, &result );
                 !measurement )
            {
                goto ABORT;
            }
            else
            {
                data = *measurement;

                // If data measurement was successful but has weak/invalid data,
                // it will retry capture for few times.
                if ( result == STATUS_OK )
                    break;
            }
        }

        // 2.a. Fill buffer
        {
            // If progress direction is in reverse, fill buffer from the
            // backside to keep pixel order.
            int bufidx
              = dir > 0 ? desc.NumPxls : NumMaxBufferedPxl - desc.NumPxls - 1;

            pixels[bufidx] = data;
            desc.NumPxls++;
        }

        // 2.b. If valid, queue motor movement
        // asynchronously Progress one pixel
        bool const bIsLineEnd
          = pos.x + dir == -1 || pos.x + dir == resolution.x;
        bool const bShouldFlush
          = bIsLineEnd || desc.NumPxls == NumMaxBufferedPxl;

        // Queue motor movement firstly
        if ( bIsLineEnd )
        {
            // Instead of calculating new pos.y value here, this sequence delays
            // calculation to identify correct desc.LineIdxk
            Motor_MoveTo( gMotY, ofst.y + steps.y * ( pos.y + 1 ), NULL, NULL );
        }
        else
        {
            // Same method with above. This is for handling a flush request
            Motor_MoveTo(
              gMotX, ofst.x + steps.x * ( pos.x + dir ), NULL, NULL );
        }

        // 3. Check transmit qualification
        if ( bShouldFlush )
        {
            bool const bIsFwd = dir == DIR_FWD;
            desc.LineIdx      = pos.y;
            desc.OfstX        = pos.x - bIsFwd * ( desc.NumPxls - 1 );
            auto data = bIsFwd ? pixels.begin() : pixels.end() - desc.NumPxls;
            auto cmd  = ECommand::RSP_LINE_DATA;

            void const*  td[] = { &cmd, &desc, data };
            size_t const ts[] = {
              sizeof( cmd ),
              sizeof( desc ),
              desc.NumPxls * sizeof( FPxlData ) };

            API_SendHostBinaries( td, ts, 3 );
            desc.NumPxls = 0;
        }

        // 3.a. Manipulate capture state
        if ( bIsLineEnd )
        {
            dir = (dir_t)-dir;
            pos.y++;
        }
        else
        {
            pos.x += dir;
        }

        // 4. Wait until motor movement done.
        wait_motor();
    }

    // Send 'job done' notifier
    {
        auto cmd = ECommand::RSP_DONE;
        API_SendHostBinary( &cmd, sizeof( cmd ) );
    }

    API_Msgf(
      "info: capture process done. elapsed: %d us\n",
      (int)( API_GetTime_us() - BeginTime ) );

ABORT:;
    // Go back to initial position
    Motor_MoveTo( gMotX, cc.Scan_CaptureOfst.x, NULL, NULL );
    Motor_MoveTo( gMotY, cc.Scan_CaptureOfst.y, NULL, NULL );
    wait_motor();

    cc.CaptureTask = NULL;
    vTaskDelete( NULL );
}

/////////////////////////////////////////////////////////////////////////////
// Point capture process

static class PointCaptureStat
{
    static constexpr size_t AddVal[] = { 1, 1 - POINT_NUM_RDQUEUE_ELEM };

public:
    void Push( FPointReq data )
    {
        Requests[Head] = data;
        Head += AddVal[Head == POINT_NUM_RDQUEUE_ELEM - 1];
        uassert( Head != Tail ); // ASSERTION FOR DATA OVERFLOW
        cc.Point_NumPendingRequest++;
    }

    std::optional<FPointReq> Pop( void )
    {
        if ( Head == Tail )
            return {};

        auto ret = Requests[Tail];
        Tail += AddVal[Tail == POINT_NUM_RDQUEUE_ELEM - 1];
        cc.Point_NumPendingRequest--;
        return ret;
    }

private:
    using rd_queue_t     = std::array<FPointReq, POINT_NUM_RDQUEUE_ELEM>;
    rd_queue_t& Requests = reinterpret_cast<rd_queue_t&>( Capture_Buffer );
    size_t      Head, Tail;

} sPointCaptureStat;

void Task_Point( void* nouse_ )
{
    // Aliasing
    auto& s = sPointCaptureStat;

    for ( size_t i = 0; i < CAPTURE_NUM_INITIAL_DISCARDS; i++ )
    {
        if ( !TryMeasureDistance( CAPTURE_NUM_MEASUREMENT_RETRY ) )
        {
            API_Msg( "error: sensor is in incorrect state. aborting ...\n" );
            goto ABORT;
        }
    }

    for ( ;; )
    { // One loop per one request.
        // Check for escape condition
        if ( cc.bPendingStop )
        {
            break;
        }
        if ( cc.bPaused )
        {
            vTaskDelay( pdMS_TO_TICKS( 10 ) );
            continue;
        }

        // If there's any point in queue, go and capture.
        auto rq = s.Pop();

        if ( !rq )
        {
            taskYIELD();
            continue;
        }

        // Relocate motor firstly.
        Motor_MoveTo( gMotX, rq->X, NULL, NULL );
        Motor_MoveTo( gMotY, rq->Y, NULL, NULL );
        wait_motor();

        // Then start capturing
        auto meas = TryMeasureDistance( CAPTURE_NUM_MEASUREMENT_RETRY );
        if ( !meas )
        {
            API_Msg( "error: failed to trigger measurement. aborting...\n" );
            break;
        }

        FPointData data;
        data.ID = rq->ID;
        data.V  = *meas;

        auto Command = ECommand::RSP_POINT;

        void const*  TrData[] = { &Command, &data };
        size_t const TrSize[] = { sizeof Command, sizeof data };
        API_SendHostBinaries( TrData, TrSize, 2 );
    }

ABORT:;
    API_Msg( "info: shutting down the capturing progress ... \n" );
    cc.CaptureTask = NULL;
    vTaskDelete( nullptr );
}

void Task_Enqeuue( int argc, char* argv[] )
{
    if ( argc != 3 )
    {
        API_Msg(
          "error: invalid number of arguments\n"
          "   usage: capture point <id:int> <xstep:int> <ystep:int>\n" );
        return;
    }

    int args[3];

    // Parse commands
    for ( size_t i = 0; i < 3; i++ )
    {
        char* validation;
        args[i]                     = strtol( argv[i], &validation, 10 );
        bool const bIsNumericString = argv[i] != validation;

        if ( bIsNumericString == false )
        {
            API_Msgf(
              "error: invalid non-numeric argument %s ... \n", argv[i] );
            return;
        }
    }

    FPointReq req { .X = args[1], .Y = args[2], .ID = args[0] };
    sPointCaptureStat.Push( req );
}
