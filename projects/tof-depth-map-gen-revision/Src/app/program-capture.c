/// @brief
///
/// @file program-capture.c
/// @author Seungwoo Kang (ki6080@gmail.com)
/// @date 2019-12-26
/// @copyright Copyright (c) 2019. Seungwoo Kang. All rights reserved.
///
/// @details
//! @todo. Fix positional error between large and small movement of a motor.
#include <stm32f4xx_hal.h>

#include "arch/hw-interface.h"
#include "arch/irq.h"
#include "arch/transciever-usb.h"
#include "program.h"
#include "protocol.h"
#include "usbd_cdc_if.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <api/argus_api.h>
#include <arch/hw-timer.h>
#include <arch/s2pi-1.h>

#include <uEmbedded/uassert.h>

#include "scanner_protocol.h"

#include "motor.h"

#define countof( arg ) ( sizeof( arg ) / sizeof( *arg ) )

argus_hnd_t* ghSens = NULL;

//=====================================================================//
//
// State machine for capturing device
//
//=====================================================================//
typedef struct
{
    int x, y;
} point_t;
typedef enum {
    ECD_STOP         = 0,
    ECD_FWD          = 1,
    ECD_BWD          = -1,
    ECD_BUSY         = -2,
    ECD_PENDING_STOP = -3,
    ECD_PAUSED       = -4,
} cpt_status_t;

typedef enum {
    ECS_NONE,
    ECS_ON_MEASURE,
    ECS_ON_EVAL,
    ECS_ON_MOVE,
} cpt_stage_t;

static struct catpure_status {
    // Stepper motor's tick count per one pixel capture
    // Determines how dense the output is.
    point_t stepsPerCapture;

    // Stepper motor's beginning step index, units in motor steps (9/160[deg])
    point_t captureBeginOffset;

    // Number of pixels to capture. i.e. Resolution
    point_t imageSize;

    // Number of pixels captured. X is column index which is reset on every line capture.
    point_t curPos;

    // A buffer holds the output measured range data.
    struct FPxlData lineBuffer[CAPTURED_LINE_BUFFER_SIZE];

    // Number of pixels stored in the buffer.
    int numPxlBuffered;

    // Measurement delay in microseconds
    size_t measureDelayUsec;

    // Direction of capturing line.
    // If this value is zero=ECD_STOP, the device is not on capturing operation.
    // \ref cpt_status_t
    cpt_status_t state;
    cpt_status_t state_paused_bkup;
    bool         bPendingPause;

    // For handle trigger event lost error ...
    // Stores latest trigger event time.
    ltimetick_t latestMeasureTrig;

    // Capture mode
    int captureMode;

    // Motor clock speed
    int motorFastClk;
    int motorSlowClk;

    // Step angle per ticks; Only for notification!
    float stepAngleX;
    float stepAngleY;

    // Stage
    cpt_stage_t stage;

    // Time that begin first capture
    ltimetick_t beginTime;

    // Capture data structures
    void*           cptRawData;
    argus_results_t cptEvalData;
} g_inst =
    {
        .stepsPerCapture    = { 1, 1 },
        .captureBeginOffset = { 0, 0 },
        .imageSize          = { 5, 5 },
        .curPos             = { 0, 0 },
        .numPxlBuffered     = 0,
        .measureDelayUsec   = 10000,
        .state              = ECD_STOP,
        .captureMode        = ARGUS_MODE_B,
        .stepAngleX         = 1.8f / 32,
        .stepAngleY         = 1.8f / 32,
        .motorFastClk       = 3000,
        .motorSlowClk       = 1000 };

void DistanceSensor_Initialize( void* nouse_ )
{
    if ( ghSens )
        Argus_DestroyHandle( ghSens );
    ghSens = Argus_CreateHandle();

    status_t stat = Argus_Init( ghSens, S2PI_SLAVE_SENSOR );

    if ( stat != STATUS_OK ) {
        print( "error: Initializing sensor has failed. Error code = %d\n", stat );
        g_inst.state = ECD_STOP;
        Argus_DestroyHandle( ghSens );
        ghSens = NULL;
        return;
    }

    // Apply calibration
    argus_calibration_t c;
    Argus_GetDefaultCalibration( &c );
    c.EEPROM.EnableEEPROMReadout = false;

    size_t calib_res = Argus_SetCalibration( ghSens, &c );
    Argus_SetConfigurationMeasurementMode( ghSens, g_inst.captureMode );
    Argus_SetConfigurationFrameTime( ghSens, g_inst.measureDelayUsec );

    // Output result
    print( "\n\nSensor has been initialized. calibration result: %d, frame time set: %d us\n", calib_res, g_inst.measureDelayUsec );
}

static void     Scanner_Watchdog( void* nouse_ );
static status_t Scanner_CaptureTick_Callback_MeasureDone( status_t stat, void* data );
static void     Scanner_CaptureTick( void* nouse_ )
{
    enum { NUM_RETRY = 5 };
    static int retry_cnt = NUM_RETRY;
    // @note.
    // Capture sequence:
    // 1. Trigger measurement. Retry until succeed.
    // 2. Evaluate result, then proceed motor.
    // 3. Queue result to buffer.
    // 3.1. If buffer is full, transmit result to host.
    if ( ghSens == NULL ) {
        if ( retry_cnt-- ) {
            print( "Reinitialization failed. Retrying  .. %d \n", retry_cnt );
            AppQueueEvent( DistanceSensor_Initialize, 0, 0 );
            QueueEventTimer( Scanner_CaptureTick, 0, 100000 );
        }
        else {
            g_inst.stage = ECS_NONE;
            g_inst.state = ECD_STOP;
            retry_cnt    = NUM_RETRY;
        }

        return;
    }

    // Try measurement
    for ( size_t retry = 5; retry; retry-- ) {
        status_t result = Argus_TriggerMeasurement(
            ghSens,
            Scanner_CaptureTick_Callback_MeasureDone );

        // The device is simply not readied yet. retry.
        if ( result == STATUS_ARGUS_POWERLIMIT ) {
            // Yield operation for one cycle ...
            // logputs("On POWERLIMIT ... Retrying capture ... \n");
            AppQueueEvent( Scanner_CaptureTick, NULL, 0 );
            return;
        }

        if ( result == STATUS_OK ) {
            // print("Measurement Triggered --> Trig : %u, Current : %u\n", timer_nextTrigger(&g_timer), SYSTEM_TIM_TICK());
            // print("Trigger ... timnum: %d, cnt: %d, ccr: %d\n",
            //       g_timer.nodes.size,
            //       SYSTEM_TIM_INST->CNT,
            //       SYSTEM_TIM_INST->CCR1);
            if ( g_inst.stage == ECS_NONE ) {
                QueueEventTimer( Scanner_Watchdog, NULL, 100000 );
            }
            g_inst.stage             = ECS_ON_MEASURE;
            g_inst.latestMeasureTrig = GetLTime();
            goto OK;
        }

        // If device has successfully captured a sequence ...
        print( "info: code [%d] Retrying measurement ... retry %d \n", result, retry );
        switch ( result ) {
        case STATUS_BUSY:
            Argus_Abort( ghSens );
            break;
        default:
            break;
        }
    }

    // When all retries exhausted ...
    // All retries exhausted ...
    // Try reinitialize sensor.
    logputs( "error: All retries exhausted. Reinitialize sensor ... \n" );
    AppQueueEvent( DistanceSensor_Initialize, NULL, 0 );
    AppQueueEvent( Scanner_CaptureTick, NULL, 0 );

OK:;
    // Done.
}

// Forward decl.
static void     Scanner_CaptureTick_OnCaptureDone( void* ppdata );
static status_t Scanner_CaptureTick_Callback_MeasureDone( status_t stat, void* data )
{
    if ( stat != STATUS_OK ) {
        logputs( "Measurement failed ... Retrying measurement ... \n" );
        QueueEventTimer( Scanner_CaptureTick, NULL, 10000 );
        return stat;
    }
    // logputs("Measurement result received.\n");
    AppQueueEvent( Scanner_CaptureTick_OnCaptureDone, &data, sizeof( data ) );
    return stat;
}

static void Scanner_CaptureTick_OnMotorMovementFinishedFinal( void* pNumMot ); // On reset movement done.
static void Scanner_CaptureTick_OnMotorMovementFinished( void* pNumMot );
static void Scanner_CaptureTick_OnCaptureDone( void* ppdata )
{
    void*           data = *(void**)ppdata;
    argus_results_t eval;
    status_t        result;

    // Set motor callbacks
    static int NumRunningMotors = 0;

    // Set stage
    g_inst.stage = ECS_ON_EVAL;

    /// If stopping process is queued ...
    /// Discards captured data, but evaluates to prevent incorrect data release.
    if ( g_inst.state == ECD_PENDING_STOP ) {
        g_inst.state            = ECD_STOP;
        g_inst.stage            = ECS_NONE;
        g_motorX.OnMovementDone = NULL;
        g_motorY.OnMovementDone = NULL;
        Motor_SetClkAll( g_inst.motorSlowClk );
        Motor_MoveTo( &g_motorX, g_inst.captureBeginOffset.x );
        Motor_MoveTo( &g_motorY, g_inst.captureBeginOffset.y );
        print( "Stopping process... \n" );
        return;
    }

    // Motor trips roundly.
    // Once it reaches endpoint of x axis, its head moves down to y axis.
    // And then reaches both end of x and y axis, it returns to initial point.

    //- Put data into buffer
    // Check if line buffer is full
    bool const bIsFwd = g_inst.state == ECD_FWD;
    int const  idx    = bIsFwd ? g_inst.numPxlBuffered : CAPTURED_LINE_BUFFER_SIZE - 1 - g_inst.numPxlBuffered;
    uassert( idx >= 0 && idx < CAPTURED_LINE_BUFFER_SIZE );
    struct FPxlData* const pxl = &g_inst.lineBuffer[idx];
    g_inst.numPxlBuffered++;
    g_inst.curPos.x += g_inst.state;

    //- Handle next motor movement

    // When motor reaches endpoint...
    bool const bAtEndpoint = g_inst.curPos.x == 0 || g_inst.curPos.x == g_inst.imageSize.x;
    bool       bIsDone     = false;
    int const  LineIdx     = g_inst.curPos.y; // Cached to be used later
    if ( bAtEndpoint ) {
        // If it is end of the line ...
        if ( ++g_inst.curPos.y == g_inst.imageSize.y ) {
            // Move motor to initial position
            g_inst.state            = ECD_STOP;
            g_inst.stage            = ECS_NONE;
            g_motorX.OnMovementDone = NULL;
            g_motorY.OnMovementDone = NULL;
            Motor_SetClkAll( g_inst.motorSlowClk );
            Motor_MoveTo( &g_motorX, g_inst.captureBeginOffset.x );
            Motor_MoveTo( &g_motorY, g_inst.captureBeginOffset.y );
            bIsDone = true;
        }
        else {
            g_inst.stage = ECS_ON_MOVE;

            g_inst.state               = -g_inst.state;
            NumRunningMotors           = 1;
            g_motorY.OnMovementDone    = Scanner_CaptureTick_OnMotorMovementFinished;
            g_motorY.OnMovementDoneObj = &NumRunningMotors;
            Motor_SetClkAll( g_inst.motorSlowClk );
            Motor_MoveBy( &g_motorY, g_inst.stepsPerCapture.y );

#if 0
            NumRunningMotors           = 2;
            g_motorX.OnMovementDone    = Scanner_CaptureTick_OnMotorMovementFinished;
            g_motorX.OnMovementDoneObj = &NumRunningMotors;
            Motor_MoveBy(&g_motorX, g_inst.stepsPerCapture.x * g_inst.state);
#endif
        }
    }
    else { // Simply move head forward
        g_inst.stage               = ECS_ON_MOVE;
        NumRunningMotors           = 1;
        g_motorX.OnMovementDone    = Scanner_CaptureTick_OnMotorMovementFinished;
        g_motorX.OnMovementDoneObj = &NumRunningMotors;
        Motor_SetClkAll( g_inst.motorFastClk );
        Motor_MoveBy( &g_motorX, g_inst.stepsPerCapture.x * g_inst.state );
    }

    // Evaluate data
    result = Argus_EvaluateData( ghSens, &eval, data );

    if ( result != STATUS_OK ) {
        logputs( "error: Data evaluation failed ... abort current process.\n" );
        g_inst.state = ECD_STOP;
        return;
    }

    pxl->Distance = eval.Bin.Range;
    pxl->AMP      = eval.Bin.Amplitude;

    //- Transfer line data if the buffer is fulfilled or the head is reached x-axis endpoint.
    // Data transfer is performed after reserving the motor movement to improve performance..
    bool const bTransmit = bAtEndpoint || g_inst.numPxlBuffered == CAPTURED_LINE_BUFFER_SIZE;
    if ( bTransmit ) {
        struct FLineData desc;
        struct FPxlData* array;
        array        = bIsFwd ? g_inst.lineBuffer : pxl;
        desc.OfstX   = bIsFwd ? g_inst.curPos.x - g_inst.numPxlBuffered : g_inst.curPos.x;
        desc.LineIdx = LineIdx;
        desc.NumPxls = g_inst.numPxlBuffered;

        // Make transmit info
        // [PACKET:CMD:DATA HEADER:DATA(VARLEN)]
        SCANNER_COMMAND_TYPE cmd    = RSP_LINE_DATA;
        size_t const         datlen = desc.NumPxls * sizeof( struct FPxlData );
        size_t const         totlen = sizeof( cmd ) + sizeof( desc ) + datlen;
        packetinfo_t         packet = PACKET_MAKE( false, totlen );

        // Transmit data through transceiver interface
        int res = 0;
        res += TransmitToHost( (char const*)&packet, sizeof( packet ) );
        res += TransmitToHost( (char const*)&cmd, sizeof( cmd ) );
        res += TransmitToHost( (char const*)&desc, sizeof( desc ) );
        res += TransmitToHost( (char const*)array, datlen );

        if ( res != totlen + sizeof( packet ) ) {
            print( "error: Only %d bytes of data from %d bytes of data is written to buffer.\n", res, totlen + sizeof( packet ) );
        }

        // Clear buffer.
        g_inst.numPxlBuffered = 0;
    }

    //- Send end signal.
    if ( bIsDone ) {
        SCANNER_COMMAND_TYPE cmd    = RSP_DONE;
        packetinfo_t         packet = PACKET_MAKE( false, sizeof( cmd ) );

        // Transmit signal
        int res = TransmitToHost( &packet, sizeof( packet ) );
        res += TransmitToHost( &cmd, sizeof( cmd ) );
        if ( res != sizeof( packet ) + sizeof( cmd ) ) {
            print(
                "error: Only %d bytes of data from"
                " %d bytes of data is written to the buffer. \n",
                res, sizeof( packet ) + sizeof( cmd ) );
        }

        ltimetick_t now = GetLTime();
        print( "Done. Time spent ... %d.%06ds \n",
               ( now.sec_1k - g_inst.beginTime.sec_1k ) * 1000 + ( now.usec - g_inst.beginTime.usec ) / 1000000,
               ( now.usec - g_inst.beginTime.usec ) % 1000000 );
    }

    // print("Evaluation finished. For x y [%d %d]\n", g_inst.curPos.x, g_inst.curPos.y);
    // logputs("Eval finished.\n");
}

static void Scanner_CaptureTick_OnMotorMovementFinished( void* pNumRunMot )
{
    uassert( pNumRunMot );
    // print("Motor cb call ... %d\n", (*(int*)pNumRunMot));

    // If there is any motor still running, do nothing.
    if ( --( *(int*)pNumRunMot ) ) {
        return;
    }

    // If paused ...
    if ( g_inst.bPendingPause ) {
        g_inst.state_paused_bkup = g_inst.state;
        g_inst.state             = ECD_PAUSED;
        print( "Process paused.\n" );
        return;
    }

    // Simply queue next capturing process.
    AppQueueEvent( Scanner_CaptureTick, NULL, 0 );
}

static void Scanner_CaptureTick_OnMotorMovementFinishedFinal( void* pNumRunMot )
{
    // print("Stopping count ... %d\n", *(int*)pNumRunMot);
    if ( --( *(int*)pNumRunMot ) ) {
        return;
    }
    g_inst.state = ECD_STOP;
    logputs( "Progress done.\n" );
}

static void Scanner_Watchdog( void* initst )
{
    bool const bIsFirst = initst == NULL;
    static struct {
        int placeholder_;
    } wd_info;

    switch ( g_inst.stage ) {
    case ECS_ON_MEASURE: {
        int timeout_us = g_inst.measureDelayUsec * 20;
        if ( !bIsFirst && LTime_Diff( GetLTime(), g_inst.latestMeasureTrig ) > timeout_us ) {
            print( "Oops, seems like trigger event is lost! retrying capture ... \n" );
            AppQueueEvent( Scanner_CaptureTick, NULL, 0 );
        }
    } break;

    default:
        break;
    }

    if ( g_inst.stage != ECS_NONE ) {
        QueueEventTimer( Scanner_Watchdog, &wd_info, 100000 );
    }
}

//=====================================================================//
//
// Command handler
//
//=====================================================================//
static bool IsIdle() { return g_inst.state == ECD_STOP && g_motorX.Status == MOTOR_IDLE && g_motorY.Status == MOTOR_IDLE; }
static void ConfigCaptureOptions( int argc, char** argv );

int HandleCaptureCommand( int argc, char** argv )
{
    enum {
        UNHANDLED = 0,
        HANDLED   = 1
    };

    if ( strcmp( argv[0], "report" ) == 0 ) {
        // Report current status
        // print("Transmitting report ... \n");
        struct FDeviceStat stat;
        stat.bIsPrecisionMode     = g_inst.captureMode == ARGUS_MODE_B;
        stat.CurMotorStepX        = g_motorX.Position;
        stat.CurMotorStepY        = g_motorY.Position;
        stat.StepPerPxlX          = g_inst.stepsPerCapture.x;
        stat.StepPerPxlY          = g_inst.stepsPerCapture.y;
        stat.DegreePerStepX       = g_inst.stepAngleX;
        stat.DegreePerStepY       = g_inst.stepAngleY;
        stat.OfstX                = g_inst.captureBeginOffset.x;
        stat.OfstY                = g_inst.captureBeginOffset.y;
        stat.SizeX                = g_inst.imageSize.x;
        stat.SizeY                = g_inst.imageSize.y;
        stat.DelayPerCapture      = g_inst.measureDelayUsec;
        stat.bIsIdle              = ( g_inst.state == ECD_STOP ) && g_motorX.Status == MOTOR_IDLE && g_motorY.Status == MOTOR_IDLE;
        stat.bIsSensorInitialized = ghSens != NULL;
        stat.bIsPaused            = g_inst.state == ECD_PAUSED;
        stat.TimeAfterLaunch_us   = LTime_LDiff( GetLTime(), g_inst.state == ECD_BWD || g_inst.state == ECD_FWD
                                                               ? g_inst.beginTime
                                                               : ( ltimetick_t ){ 0u, 0u } );

        SCANNER_COMMAND_TYPE cmd = RSP_STAT_REPORT;

        packetinfo_t packet = PACKET_MAKE( false, sizeof( cmd ) + sizeof( stat ) );
        TransmitToHost( &packet, sizeof( packet ) );
        TransmitToHost( &cmd, sizeof( cmd ) );
        TransmitToHost( &stat, sizeof( stat ) );
    }
    else if ( strcmp( argv[0], "ping" ) == 0 ) {
        SCANNER_COMMAND_TYPE cmd    = RSP_PING;
        packetinfo_t         packet = PACKET_MAKE( false, sizeof( cmd ) );
        TransmitToHost( &packet, sizeof( packet ) );
        TransmitToHost( &cmd, sizeof( cmd ) );
    }
    else if ( strcmp( argv[0], "start" ) == 0 ) {
        g_inst.bPendingPause = false;

        if ( g_inst.state == ECD_FWD || g_inst.state == ECD_BWD ) {
            // logputs("Device is busy! !\n");
            print( "Requesting pause ... \n" );
            g_inst.bPendingPause = true;
            return HANDLED;
        }
        if ( g_inst.state == ECD_PAUSED ) {
            print( "Requesting resume ... \n" );
            g_inst.state = g_inst.state_paused_bkup;
            Motor_SetClkAll( g_inst.motorFastClk );
            Scanner_CaptureTick( NULL );
            return HANDLED;
        }
        if ( ghSens == NULL ) {
            logputs( "warning: Sensor is not initialized.\n" );
            DistanceSensor_Initialize( NULL );
        }

        g_inst.beginTime = GetLTime();
        /// @todo.
        // - Verify parameter validity. (e.g. Image size value 30000 that is out of maximum step range 1000)

        // Move motor to initial position.
        // This is done synchronously.
        print( "Moving motor head to initial position ... \n" );
        void       DelayedTriggerCaptureCallback( void* );
        static int NumRunningMotors;
        g_inst.state               = ECD_BUSY;
        NumRunningMotors           = 2;
        g_motorX.OnMovementDone    = DelayedTriggerCaptureCallback;
        g_motorY.OnMovementDone    = DelayedTriggerCaptureCallback;
        g_motorX.OnMovementDoneObj = &NumRunningMotors;
        g_motorY.OnMovementDoneObj = &NumRunningMotors;
        Motor_SetClkAll( g_inst.motorSlowClk );
        Motor_MoveTo( &g_motorX, g_inst.captureBeginOffset.x );
        Motor_MoveTo( &g_motorY, g_inst.captureBeginOffset.y );
    }
    else if ( strcmp( argv[0], "motor-move" ) == 0 ) {
        if ( !IsIdle() ) {
            print( "warning: Motor movement request ignored.\n" );
            return HANDLED;
        }
        int x = 0, y = 0;
        if ( argc >= 2 )
            x = atoi( argv[1] );
        if ( argc >= 3 )
            y = atoi( argv[2] );

        print( "Moving motor by %d %d\n", x, y );
        static int NumRunningMotors;
        void       MotorMoveCallback( void* );
        NumRunningMotors           = 2;
        g_motorX.OnMovementDone    = MotorMoveCallback;
        g_motorY.OnMovementDone    = MotorMoveCallback;
        g_motorX.OnMovementDoneObj = &NumRunningMotors;
        g_motorY.OnMovementDoneObj = &NumRunningMotors;
        g_inst.state               = ECD_BUSY;
        Motor_SetClkAll( g_inst.motorSlowClk );
        Motor_MoveBy( &g_motorX, x );
        Motor_MoveBy( &g_motorY, y );
        g_motorX.PendingTicks = 0;
        g_motorY.PendingTicks = 0;
    }
    else if ( strcmp( argv[0], "config" ) == 0 ) {
        if ( g_inst.state != ECD_STOP ) {
            logputs( "Configuration is only available when the motor is stopped.\n" );
            return HANDLED;
        }
        ConfigCaptureOptions( argc - 1, argv + 1 );
    }
    else if ( strcmp( argv[0], "stop" ) == 0 ) {
        if ( g_inst.state == ECD_FWD || g_inst.state == ECD_BWD ) {
            g_inst.state = ECD_PENDING_STOP;
            return HANDLED;
        }
        if ( g_inst.state == ECD_PAUSED ) {
            g_inst.state            = ECD_STOP;
            g_motorX.OnMovementDone = NULL;
            g_motorY.OnMovementDone = NULL;
            Motor_SetClkAll( g_inst.motorFastClk );
            Motor_MoveTo( &g_motorX, g_inst.captureBeginOffset.x );
            Motor_MoveTo( &g_motorY, g_inst.captureBeginOffset.y );
            return HANDLED;
        }
        if ( g_inst.state == ECD_BUSY ) {
            g_inst.state = ECD_STOP;
            Motor_Stop_Emergency( &g_motorX );
            Motor_Stop_Emergency( &g_motorY );
        }
    }
    else if ( strcmp( argv[0], "init-sensor" ) == 0 ) {
        if ( g_inst.state != ECD_STOP ) {
            logputs( "Can't invalidate sensor during running.\n" );
            return HANDLED;
        }

        // -- Initialize device
        // Only available in idle state
        // Reset sensor
        DistanceSensor_Initialize( NULL );
    }
    else if ( strcmp( argv[0], "motor-reset" ) == 0 ) {
        if ( g_inst.state != ECD_STOP ) {
            logputs( "Motor can be reset only when there is no process.\n" );
            return HANDLED;
        }
        g_motorX.Position     = 0;
        g_motorY.Position     = 0;
        g_motorX.PendingTicks = 0;
        g_motorY.PendingTicks = 0;
    }
    else if ( strcmp( argv[0], "motor-relocate" ) == 0 ) {
        // Only available when the motor is active.
        if ( g_motorY.Status != MOTOR_IDLE || g_motorX.Status != MOTOR_IDLE ) {
            logputs( "Yet motor is busy.\n" );
            return HANDLED;
        }

        // Relocate motor head by using proximity sensor
        /// @todo. Move motor until it reaches leftmost-topmost endpoint using proximity sensor.
        static int NumRunningMotors;
        NumRunningMotors = 2;
        void MotorMoveCallback( void* );
        g_motorX.OnMovementDone    = MotorMoveCallback;
        g_motorY.OnMovementDone    = MotorMoveCallback;
        g_motorX.OnMovementDoneObj = &NumRunningMotors;
        g_motorY.OnMovementDoneObj = &NumRunningMotors;
        g_inst.state               = ECD_BUSY;
        Motor_SetClkAll( g_inst.motorSlowClk );
        Motor_MoveTo( &g_motorX, 0 );
        Motor_MoveTo( &g_motorY, 0 );
    }
    else if ( strcmp( argv[0], "help" ) == 0 ) {
        print(
            " --- DEPTH SCAN SYSTEM v1.0 --- \n"
            " usage: <command> [args...] \n"
            " command list: \n"
            "   report              Update report\n"
            "   ping                Request ping\n"
            "   start               Request start, or pause, or resume.\n"
            "   stop                Request stop\n"
            "   motor-move [x [y]]  Request motor movement. Units are in step.\n"
            "   config [args]       Configure arguments; more: try type \"config\" only.\n"
            "   init-sensor         Request initialize distance sensor.\n"
            "   motor-reset         Set this position as motor origin\n"
            "   motor-relocate      Relocate motor to origin \n"
            "\n"
            " debug commands: \n"
            "   test-pattern        Try test SPI - Sensor linkage\n"
            "   test-timer          Try test timer\n"
            "   timer-psc           Change timer 2's PSC(pre-scaler) value.\n" );
    }
    else if ( strcmp( argv[0], "r" ) == 0 ) {
        // For debug ... log timer statistics
        print(
            " --- Timer Statistics --- \n"
            " Current GTICK   : %u \n"
            " Next GTICK      : %u \n"
            " Number of Nodes : %u \n"
            " Capture Stage   : %u \n",
            SYSTEM_TIM_TICK(),
            SYSTEM_TIM_INST->CCR1,
            g_timer.nodes.size,
            g_inst.stage );

        print(
            "--- Motor Statistics --- \n"
            " X pos           : %d \n"
            " Y pos           : %d \n",
            g_motorX.Position, g_motorY.Position );
    }
    else {
        return UNHANDLED;
    }

    // Returns 1 if handled
    return HANDLED;
}

static inline int32_t clampi32( int32_t val, int32_t min, int32_t max )
{
    return val < min ? min : val > max ? max : val;
}

static void ConfigCaptureOptions( int argc, char** argv )
{
    // No args. Invalid op.
    if ( argc == 0 ) {
        logputs( "warning: Config option is not specified.\n"
                 "[usage] <operation> [value1 [value 2 [...]]]\n"
                 "   Put -1 not to modify value.\n"
                 "Options\n"
                 "   ofst            Capture begin offset in motor steps\n"
                 "   resolution      Output image size\n"
                 "   pixel-step      Required motor steps per pixel capture\n"
                 "   delay           Capture delay in microseconds. Will be applied on next initialization.\n"
                 "   angle-per-step  Set angle per steps. Use reinterpret cast of float value. \n"
                 "   precision       Set precision mode. 0 to far, non-zero to close. \n"
                 "   motor-clk       Motor clock speed.\n" );
        return;
    }

    // Configuration is only available during idle state
    if ( g_inst.state != ECD_STOP )
        return;

    int32_t xval = -1;
    int32_t yval = -1;

    // Pre-parse optional parameters
    if ( argc >= 2 )
        xval = atoi( argv[1] );
    if ( argc >= 3 )
        yval = atoi( argv[2] );

    if ( strcmp( argv[0], "offset" ) == 0 ) {
        if ( argc >= 2 )
            g_inst.captureBeginOffset.x = xval;
        if ( argc >= 3 )
            g_inst.captureBeginOffset.y = yval;

        /// @todo. Handle error for invalid arguments ... No need ?
        /// @todo. Queue motor movement
        print( "Step offset is set to %d, %d\n", g_inst.captureBeginOffset.x, g_inst.captureBeginOffset.y );
    }
    else if ( strcmp( argv[0], "resolution" ) == 0 ) {
        if ( xval > 0 )
            g_inst.imageSize.x = xval;
        if ( yval > 0 )
            g_inst.imageSize.y = yval;
        print( "Image size is set to %d, %d\n", g_inst.imageSize.x, g_inst.imageSize.y );
    }
    else if ( strcmp( argv[0], "step-per-pixel" ) == 0 ) {
        if ( xval > 0 )
            g_inst.stepsPerCapture.x = xval;
        if ( yval > 0 )
            g_inst.stepsPerCapture.y = yval;

        // Adjust motor clock time, to make each pixels have at least 5000 microseconds for motor movement
        int lower = g_inst.stepsPerCapture.x > g_inst.stepsPerCapture.y ? g_inst.stepsPerCapture.y : g_inst.stepsPerCapture.x;
        Motor_SetClkAll( g_inst.motorFastClk );
        int interval = g_motorX.interval_us;
        if ( false && interval * lower < 2000 ) {
            // Let interval larger than 5000/lower
            // Align to 100
            interval = ( ( 2000 / lower + 50 ) / 100 ) * 100;

            int clk             = (int)1e6 / interval;
            g_inst.motorFastClk = clk;
            g_inst.motorSlowClk = clk < g_inst.motorSlowClk ? clk : g_inst.motorSlowClk;
            print( "Motor fast clock is adjusted to %d to stabilize capture result\n", clk );
        }

        print( "Steps per capture is set to %d, %d\n", g_inst.stepsPerCapture.x, g_inst.stepsPerCapture.y );
    }
    else if ( strcmp( argv[0], "angle-per-step" ) == 0 ) {
        float x = *(float*)&xval;
        float y = *(float*)&yval;
        if ( argc >= 2 && x > 0.f )
            g_inst.stepAngleX = x;
        if ( argc >= 3 && y > 0.f )
            g_inst.stepAngleY = y;
        print( "Degrees per step is set to %f, %f\n", x, y );
    }
    else if ( strcmp( argv[0], "delay" ) == 0 ) {
        if ( xval > 0 )
            g_inst.measureDelayUsec = clampi32( xval, 1000, 0xfffff );
        // Set frame time.
        Argus_SetConfigurationFrameTime( ghSens, g_inst.measureDelayUsec );
        print( "Delay is set to %d\n", g_inst.measureDelayUsec );
    }
    else if ( strcmp( argv[0], "precision" ) == 0 ) {
        g_inst.captureMode = xval == 0 ? ARGUS_MODE_A : ARGUS_MODE_B;
        Argus_SetConfigurationMeasurementMode( ghSens, g_inst.captureMode );
    }
    else if ( strcmp( argv[0], "motor-clk" ) == 0 ) {
        if ( argc >= 2 && xval > 0 )
            g_inst.motorFastClk = xval;

        print( "Setting motor fast clock as %d ... \n", xval );
    }
    else if ( strcmp( argv[0], "motor-clk-on-large-move" ) == 0 ) {
        if ( argc >= 2 && xval > 0 ) {
            // Maximum is fastclk value.
            g_inst.motorSlowClk = xval < g_inst.motorFastClk ? xval : g_inst.motorFastClk;
        }

        print( "Setting motor slow clock as %d ... \n", g_inst.motorSlowClk );
    }
    else {
        print( "warning: Unknown config option [%s].\n", argv[0] );
    }
}

void DelayedTriggerCaptureCallback( void* pmotrun )
{
    int* pNumRunningMot = pmotrun;
    print( "Delayed capture initialize: %d\n", *pNumRunningMot );

    if ( --( *pNumRunningMot ) == 0 ) {
        /// Reset parameters to initial value
        g_inst.numPxlBuffered = 0;
        g_inst.curPos.x       = 0;
        g_inst.curPos.y       = 0;

        // Run Capture
        g_inst.state = ECD_FWD;
        QueueEvent( &g_event, Scanner_CaptureTick, NULL, 0 );
        Motor_SetClkAll( g_inst.motorFastClk );
    }
}

void MotorMoveCallback( void* numrunmot )
{
    int* NumRunningMotors = numrunmot;
    if ( --*NumRunningMotors > 0 )
        return;

    g_inst.state = ECD_STOP;
}
