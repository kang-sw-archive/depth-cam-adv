/**
 * @brief 
 * 
 * @file program.c
 * @author Seungwoo Kang (ki6080@gmail.com)
 * @version 0.1
 * @date 2019-12-25
 * @copyright Copyright (c) 2019. Seungwoo Kang. All rights reserved.
 * 
 * @details
 * All the communication functionalities are handled inside of this file, which arbitrates multiple concurrent data transfer requests.
 **/
#include <stm32f4xx_hal.h>

#include "arch/hw-interface.h"
#include "arch/irq.h"
#include "arch/transciever-usb.h"
#include "motor.h"
#include "usbd_cdc_if.h"

#include "program.h"
#include "protocol.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <api/argus_api.h>
#include <arch/hw-timer.h>
#include <arch/s2pi-1.h>

#include <uEmbedded/uassert.h>

// --
// --
// -- GLOBAL VARIABLES
// --
// --
typedef bool const cbool;

queue_buffer_t                g_recvQueue; // Receive queue.
static struct usb_transceiver g_usbTr;     // USB transceiver instance.
struct EventQueue             g_event;     // Event queue
transceiver_vptr_t            g_trans;     // Transceiver reference for adaptive operation
static char                   RECV_QUEUE_BUFFER__[RECEIVE_QUEUE_BUFER_SIZE];
static char                   EVENT_BUFFER__[EVENT_QUEUE_BUFFER_SIZE];
static char                   TIMER_BUFFER__[TIMER_BUFFER_SIZE];
uint32_t                      NUM_1KSEC;
motor_t                       g_motorX;
motor_t                       g_motorY;
int                           gMotorDriveClk;

void ( *EXTI_CALLBACK_LINE[16] )( void ); // Callback handlers per EXTI lines.

// --
// --
// -- INTERRUPT HANDLERS
// --
// --
// USB interrupt handler
void HandleCdcInputData( uint8_t* Buf, uint32_t* Len )
{
    queue_buffer_push( &g_recvQueue, Buf, *Len );
}

// EXTI interrupt handler
void HAL_GPIO_EXTI_Callback( uint16_t GPIO_Pin )
{
    for ( size_t i = 0; GPIO_Pin; GPIO_Pin >>= 1, ++i ) {
        if ( ( GPIO_Pin & 1 ) && EXTI_CALLBACK_LINE[i] != NULL ) {
            EXTI_CALLBACK_LINE[i]();
        }
    }
}

// --
// --
// -- PROGRAM UTILITY
// --
// --
static char   g_logTextQueue[LOGGING_TEXT_QUEUE_BUFFER_SIZE];
static size_t g_logTextHead = 0;
static char   TRANSMIT_BUFFER[TRANSMIT_QUEUE_BUFFER_SIZE];
static size_t g_transmit_head = 0;

void FlushLogText();

int TransmitToHost( void* data, size_t datlen )
{
    memcpy( TRANSMIT_BUFFER + g_transmit_head, data, datlen );
    g_transmit_head += datlen;
    return datlen;
}

static void FlushTransmitData()
{
    if ( g_transmit_head == 0 )
        return;
    g_trans->write( g_trans, TRANSMIT_BUFFER, g_transmit_head );
    g_transmit_head = 0;
}

void vlogprintf( char const* str, va_list vp )
{
    size_t num = vsnprintf( NULL, 0, str, vp );
    if ( g_logTextHead + num >= sizeof( g_logTextQueue ) ) {
        *(volatile int*)( 0 );
        return;
    }

    g_logTextHead += vsprintf( g_logTextQueue + g_logTextHead, str, vp );

    if ( g_logTextHead > sizeof( g_logTextQueue ) / 2 && irq_lock_ct == 0 ) {
        FlushLogText();
    }
}

void logputs( const char* str )
{
    while ( *str ) {
        g_logTextQueue[g_logTextHead] = *str++;
        ++g_logTextHead;
    }
}

void logprint( char const* str, ... )
{
    va_list vp;
    va_start( vp, str );
    vlogprintf( str, vp );
    va_end( vp );
}

void FlushLogText()
{
    if ( g_logTextHead == 0 )
        return;

    packetinfo_t pack;
    pack = PACKET_MAKE( true, g_logTextHead );

    TransmitToHost( (char*)&pack, sizeof( packetinfo_t ) );
    TransmitToHost( g_logTextQueue, g_logTextHead );

    g_logTextHead = 0;
}

// Event Timer logics
typedef struct
{
    void ( *cb )( void* );
    void* cbo;
} cb_t;
static struct fslist g_queueTimerList;
static char          QUEUE_TIMER_LIST_BUFF[EVENT_TIMER_QUEUE_BUFFER_SIZE];

static void QueueEventTimerInit()
{
    fslist_init( &g_queueTimerList, QUEUE_TIMER_LIST_BUFF, sizeof( QUEUE_TIMER_LIST_BUFF ), sizeof( cb_t ) );
}

static void event_cb__( void* cbo )
{
    struct fslist_node* n = *(struct fslist_node**)cbo;

    // Pop callback information and release memory.
    irq_lock();
    cb_t v = *(cb_t*)fslist_data( &g_queueTimerList, n );
    fslist_erase( &g_queueTimerList, n );
    irq_unlock();

    // Call callback
    v.cb( v.cbo );
}

void AppQueueEvent( void ( *eventCallback )( void* ), void* callbackObj, size_t Size )
{
    irq_lock();
    QueueEvent( &g_event, eventCallback, callbackObj, Size );
    irq_unlock();
}

static void timer_cb__proc( void* cbo )
{
    AppQueueEvent( event_cb__, &cbo, sizeof( intptr_t ) );
}

timer_handle_t QueueEventTimer( void ( *cb )( void* ), void* cbo, uint32_t microSec )
{
    uassert( cb );
    uassert( microSec );

    // Reserve space for callback information
    irq_lock();
    struct fslist_node* n = fslist_insert( &g_queueTimerList, NULL );
    uassert( n );

    // Copy callback information into node data.
    *(cb_t*)fslist_data( &g_queueTimerList, n ) = ( cb_t ){ .cb = cb, .cbo = cbo };

    timer_handle_t ret = QueueTimer( timer_cb__proc, n, microSec );
    irq_unlock();

    // timer_info_t const *info = timer_browse(&g_timer, ret);
    // print( "Queueing timer for interval %d ... timerId: %d Trigger: %d, Now: %d\n", microSec, info->timerId, info->triggerTime, SYSTEM_TIM_TICK() );
    return ret;
}

void Motor_SetClkAll( int clk )
{
    if ( gMotorDriveClk != clk ) {
        gMotorDriveClk     = clk;
        cbool       xres   = Motor_SetClk( &g_motorX, clk );
        cbool       yres   = Motor_SetClk( &g_motorY, clk );
        const char* msg[2] = { "failed", "success" };
        // print( "Switching motor clock speed to %d ... \n", clk );
        // print( "  result          x[ %8s ] y[ %8s ] \n", msg[xres], msg[yres] );
        // print( "  interval is ... x[ %8d ] y[ %8d ] \n", g_motorX.interval_us, g_motorY.interval_us );
    }
}

void AbortEventTimer( timer_handle_t hnd )
{
    irq_lock();
    timer_info_t const* info = timer_browse( &g_timer, hnd );
    if ( info == NULL ) {
        goto END;
    }
    void* const cbo = info->callbackObj;
    uassert( cbo );
    struct fslist_node* n = cbo;
    fslist_erase( &g_queueTimerList, n );
    AbortTimer( hnd );
END:;
    irq_unlock();
}

status_t print( const char* str, ... )
{
    ltimetick_t now = GetLTime();
    logprint( "[%d.%06d s] ", now.sec_1k * 1000 + now.usec / 1000000, now.usec % 1000000 );
    va_list vp;
    va_start( vp, str );
    vlogprintf( str, vp );
    va_end( vp );

    return STATUS_OK;
}

void uemb_assert_impl_func( char const* expr, char const* file, char const* function, int line )
{
    print( "ASSERT FAILED!" );
    while ( 1 ) {}
}

void SplitStringBySpace( char* begin, char** o_argv, int* o_argc, size_t capacity )
{
    // Skips all initial spaces
    while ( *begin == ' ' ) {
        ++begin;
    }

    *o_argc             = 0;
    volatile char* head = begin;

    for ( ;; ) {
        if ( ( *head == ' ' || *head == '\0' ) && head != begin ) {
            // Make new split string by inserting null character.
            *o_argv++ = begin;
            ++*o_argc;

            if ( *head == '\0' || *o_argc >= capacity )
                break;

            *head = '\0';
            // Advance head pointer until encounter non-space character.
            while ( *++head == ' ' ) {}

            // Begin position should be non-space character also.
            begin = head;
        }
        else {
            ++head;
        }
    }
}

static void irq_lock_wrap( void* nouse_ ) { irq_lock(); }
static void irq_unlock_wrap( void* nouse_ ) { irq_unlock(); }
// --
// --
// -- MAIN LOOP
// --
// --

void appMain()
{
    // Initialize sequence.
    static char buf[1024];
    uemb_assert_impl = uemb_assert_impl_func;
    S2PI_Init();
    queue_buffer_init( &g_recvQueue, RECV_QUEUE_BUFFER__, sizeof( RECV_QUEUE_BUFFER__ ) );
    InitEventProcedure( &g_event, EVENT_BUFFER__, sizeof( EVENT_BUFFER__ ) );
    InitUsbTransceiver( &g_usbTr );
    timer_init( &g_timer, TIMER_BUFFER__, sizeof( TIMER_BUFFER__ ) );
    QueueEventTimerInit();
    InitMotors();
    Motor_SetClkAll( 1000 );

    // -- Program loop ... never breaks !!
    while ( true ) {
        //! @todo. Implement checking network connection process.
        if ( false /*Check for ethernet DHCP condition ... */ ) {
        }
        else if ( IsUsbActive() ) {
            g_trans = g_usbTr.vtable;
        }
        else // If no connection is detected ...
            continue;

        // Connection successful.
        logputs( "Connection successful.\n" );

        // -- During this connection is alive ...
        transceiver_result_t transceiver_result;
        while ( ( transceiver_result = g_trans->status( g_trans ) ) != TRANSCEIVER_NO_CONNECTION ) {
            // Check if there's anything to read.
            // If so, handle it firstly.
            if ( transceiver_result >= 4 ) {
                packetinfo_t info;

                // Read and check for header.
                queue_buffer_draw( &g_recvQueue, &info, sizeof( packetinfo_t ) );

                // If header is invalid ...
                if ( !PACKET_IS_PACKET( info ) ) {
                    // Reset input buffer and wait.
                    HAL_Delay( 250 );
                    queue_buffer_init( &g_recvQueue, RECV_QUEUE_BUFFER__, sizeof( RECV_QUEUE_BUFFER__ ) );
                    continue;
                }

                // Pop the rest of data.
                size_t length = PACKET_LENGTH( info );
                while ( queue_buffer_size( (volatile*)&g_recvQueue ) < length ) {}
                queue_buffer_draw( &g_recvQueue, buf, PACKET_LENGTH( info ) );

                void StringHandler( char* str );
                void BinaryHandler( char const* data, size_t len );

                if ( PACKET_IS_STR( info ) ) {
                    buf[length] = '\0';
                    StringHandler( buf );
                }
                else
                    BinaryHandler( buf, length );
            }

            // Process queued events
            ProcessEvent( &g_event, irq_lock_wrap, irq_unlock_wrap, NULL );

            // Flush all log text
            FlushLogText();

            // Perform manual timer update
            HWTimerUpdate();

            // Flush all data to host
            FlushTransmitData();
        }

        // Close current transceiver to reopen another device.
        g_trans->close( g_trans );
    }
}

// --
// --
// -- STATE MACHINE OF DEVICE
// --
// --

// --
// --
// -- INPUT HANDLER
// --
// --

//=====================================================================//
//
// <+> Test static functions
//
//=====================================================================//
int TestStringHandler( int argc, char** argv );
int HandleCaptureCommand( int argc, char** argv );
#define countof( v ) ( sizeof( v ) / sizeof( *v ) )

void StringHandler( char* str )
{
    enum {
        UNHANDLED = 0,
        HANDLED   = 1
    };

    int   argc;
    char* argv[16];

    if ( *str == 0 )
        return;

    SplitStringBySpace( str, argv, &argc, countof( argv ) );

    // If command has handled inside of the function, finish op...
    if ( argc &&
         ( HandleCaptureCommand( argc, argv ) == UNHANDLED &&
           TestStringHandler( argc, argv ) == UNHANDLED ) ) {
        logprint( "Invalid command %s\n", argv[0] );
    }
}

void BinaryHandler( char const* data, size_t len )
{
}
