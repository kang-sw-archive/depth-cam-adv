#include "app.h"
#include "defs.h"
#include "protocol.h"
#include "rw.h"
#include <alloca.h>
#include <semphr.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uEmbedded-pp/utility.hxx>
#include <uEmbedded/uassert.h>
/////////////////////////////////////////////////////////////////////////////
// Host IO communication handler
//
//

/////////////////////////////////////////////////////////////////////////////
// Utilities

// Command procedure
static void stringCmdHandler( char* str, size_t len );
static void binaryCmdHandler( char* data, size_t len );
// Token parser
static int stringToTokens( char* str, char* argv[], size_t argv_len );

// Flush buffered data to host
static char          s_hostTrBuf[HOST_TRANSFER_BUFFER_SIZE];
static size_t        s_hostTrBufHead = 0;
static volatile int  s_writingTask   = 0;
static volatile bool s_bFlushing     = 0;
static void          apndToHostBuf( void const* d, size_t len );
static void          flushTransmitData();

// Read host connection for requested byte length.
// @returns false when failed to receive data, with given timeout.
static bool readHostConn( void* dst, size_t len );

/////////////////////////////////////////////////////////////////////////////
// Primary Procedure
extern "C" _Noreturn void AppProc_HostIO( void* nouse_ )
{
    packetinfo_t packet;

    for ( ;; ) {
        // Check if read data has valid protocol.
        if ( readHostConn( &packet, PACKET_SIZE ) == false )
            continue;

        // Check packet validity
        // If any data were delivered in bad condition, it'll consume all pending bytes.
        if ( PACKET_IS_PACKET( packet ) == false )
            continue;

        // Should be aware of maximum stack depth!
        // Packet size must be less than 2kByte at once
        // Allocate packet receive memory using VLA
        auto len = PACKET_LENGTH( packet );
        char buf[len + 1];
        if ( readHostConn( buf, len ) == false )
            continue;

        // Call command procedure
        ( PACKET_IS_STR( packet ) ? stringCmdHandler : binaryCmdHandler )( buf, len );
    }
}

/////////////////////////////////////////////////////////////////////////////
// Global function defs
void API_SendHostBinary( void const* data, size_t len )
{
    packetinfo_t packet = PACKET_MAKE( false, len );
    apndToHostBuf( &packet, sizeof packet );
    apndToHostBuf( data, len );
}

void API_SendHostString( void const* data, size_t len )
{
    packetinfo_t packet = PACKET_MAKE( true, len );
    apndToHostBuf( &packet, sizeof packet );
    apndToHostBuf( data, len );
}

void API_SendHostRaw( void const* data, size_t len )
{
    apndToHostBuf( data, len );
}

void print( char const* fmt, ... )
{
    va_list vp;

    va_start( vp, fmt );
    size_t allocsz = vsnprintf( NULL, 0, fmt, vp ) + 2;
    va_end( vp );

    va_start( vp, fmt );
    char* buf = (char*)alloca( allocsz );
    vsprintf( buf, fmt, vp );
    va_end( vp );

    API_SendHostString( buf, allocsz );
}

/////////////////////////////////////////////////////////////////////////////
// Utility defs
bool readHostConn( void* dst, size_t len )
{
    size_t numRetries = 5;
    for ( ; numRetries; ) {
        // Flush transmit data before try read
        flushTransmitData();

        auto result = td_read( gHostConnection, (char*)dst, len );

        // If there's nothing to read, sleep for 1 cycle
        if ( result == 0 ) {
            vTaskDelay( 1 );
            continue;
        }
        else if ( result < 0 ) {
            vTaskDelay( pdMS_TO_TICKS( 10 ) );
            --numRetries;
        }

        len -= result;
        dst = (char*)dst + result;

        if ( len == 0 ) {
            return true;
        }
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////
//
extern "C" bool App_HandleCaptureCommand( int argc, char* argv[] );

void stringCmdHandler( char* str, size_t len )
{
    // Append last byte as null ch
    str[len + 1] = '\0';

    // Make tokens from string ... Maximum token = 16
    char* argv[16];
    int   argc = stringToTokens( str, argv, sizeof( argv ) / sizeof( *argv ) );

    if ( argc == 0 )
        return;

#define STRCASE( v ) upp::hash::fnv1a_32( v )
    uint32_t cmdidx = STRCASE( argv[0] );

    switch ( cmdidx ) {
    case STRCASE( "app-os-report" ):

        break;

    case STRCASE( "test-input" ):
        print( "Hello, world!\n" );
        break;

    default:
        App_HandleCaptureCommand( argc, argv );
        break;
    }
}

extern "C" __weak_symbol bool
App_HandleCaptureCommand( int argc, char* argv[] )
{
    print( "Tokens :: \n" );
    for ( int i = 0; i < argc; i++ ) {
        print( "  %02d: %s\n", i, argv[i] );
    }
    return true;
}

void binaryCmdHandler( char* data, size_t len )
{
}

int stringToTokens( char* str, char* argv[], size_t argv_len )
{
    // Consume all initial spaces
    while ( *str == ' ' )
        ++str;

    // Ignore space-only string.
    if ( *str == 0 )
        return 0;

    int   num_token = 0;
    char* head      = str;

    for ( ; num_token < (int)argv_len; ) {
        if ( *head != ' ' && *head != '\0' ) {
            ++head;
            continue;
        }

        argv[num_token++] = str;
        if ( *head == 0 )
            break;

        for ( *head = 0; *++head == ' '; ) { }

        if ( *head == 0 )
            break;

        str = head; // Non-space character pos
    }

    return num_token;
}

void apndToHostBuf( void const* d, size_t len )
{
    uassert( s_hostTrBufHead + len < sizeof( s_hostTrBuf ) );

    while ( s_bFlushing )
        taskYIELD();

    ++s_writingTask;
    s_hostTrBufHead += len;
    memcpy( s_hostTrBuf + s_hostTrBufHead - len, d, len );
    --s_writingTask;
}

void flushTransmitData()
{
    if ( s_hostTrBufHead == 0 )
        return;

    // Wait for all async write process done
    while ( s_writingTask > 0 )
        taskYIELD();

    s_bFlushing = true;
    td_write( gHostConnection, s_hostTrBuf, s_hostTrBufHead );
    s_hostTrBufHead = 0;
    s_bFlushing     = false;
}
