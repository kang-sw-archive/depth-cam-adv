#include <FreeRTOS.h>

#include <alloca.h>
#include <semphr.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uEmbedded-pp/utility.hxx>
#include <uEmbedded/algorithm.h>
#include <uEmbedded/uassert.h>
#include "../defs.h"
#include "../protocol/protocol-s.h"
#include "../protocol/protocol.h"
#include "app.h"
#include "hal.h"
/////////////////////////////////////////////////////////////////////////////
// Defines host IO communication handler
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
        // If any data was delivered in bad condition, it'll consume all pending
        // bytes.
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
        ( PACKET_IS_STR( packet ) ? stringCmdHandler : binaryCmdHandler )(
          buf, len );
    }
}

/////////////////////////////////////////////////////////////////////////////
// Global function defs
extern "C" {
void API_SendHostBinary( void const* data, size_t len )
{
    packetinfo_t packet = PACKET_MAKE( false, len );
    apndToHostBuf( &packet, sizeof packet );
    apndToHostBuf( data, len );
}

void API_SendHostBinaries( void const* const data[],
  size_t const                               len[],
  size_t                                     cnt )
{
    size_t sum = 0;
    for ( size_t i = 0; i < cnt; i++ )
        sum += len[i];

    portENTER_CRITICAL();
    packetinfo_t p = PACKET_MAKE( 0, sum );
    API_SendHostRaw( &p, sizeof( p ) );
    for ( size_t i = 0; i < cnt; i++ )
        API_SendHostRaw( data[i], len[i] );
    portEXIT_CRITICAL();
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

static struct export_data
{
    struct node
    {
        uint32_t    id_;
        void const* mem_;
        size_t      len_;
    } node_[NUM_MAX_EXPORT_BINARY];
    size_t     size_ = 0;
    static int compare( void const* a, void const* b )
    {
        return reinterpret_cast<node const*>( a )->id_
               - reinterpret_cast<node const*>( b )->id_;
    }
} s_xd;

void API_ExportBin( uint32_t id, void const* mem, size_t len )
{
    using ed      = export_data::node;
    ed* const arr = s_xd.node_;

    auto idx
      = lowerbound( arr, &id, sizeof( ed ), s_xd.size_, export_data::compare );
    auto head = arr + idx;

    if ( head->id_ != id ) {
        uassert( idx < NUM_MAX_EXPORT_BINARY );
        head = (ed*)array_insert( arr, NULL, idx, sizeof( ed ), &s_xd.size_ );
        head->id_ = id;
    }

    head->len_ = len;
    head->mem_ = mem;
}

void API_RemoveExport( uint32_t id, void const* ptr )
{
    //! @warning. Should not call this!
    uassert( false );
    using ed      = export_data::node;
    ed* const arr = s_xd.node_;

    auto idx
      = lowerbound( arr, &id, sizeof( ed ), s_xd.size_, export_data::compare );
    if ( arr[idx].id_ == id ) {
        //! @todo. Implement array_remove from uEmbedded ...
    }
}
}

static void ProcessGet( char const* name )
{
    // Find ref
    auto id       = upp::hash::fnv1a_32( name );
    using ed      = export_data::node;
    ed* const arr = s_xd.node_;

    auto idx
      = lowerbound( arr, &id, sizeof( ed ), s_xd.size_, export_data::compare );
    auto at = arr + idx;

    if ( at->id_ != id ) {
        API_Msgf( "Given name %s is not exported data name\n", name );
        return;
    }

    FGetDesc d;
    d.Length = at->len_;
    strncpy( d.TAG, name, SCANNER_NUM_GET_TAG_LENGTH );

    void const* dat[] = { &d, at->mem_ };
    size_t      len[] = { sizeof d, at->len_ };

    API_SendHostBinaries( dat, len, 2 );
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

void stringCmdHandler( char* str, size_t len )
{
    // Append last byte as null ch
    str[len + 1] = '\0';

    // Make tokens from string ... Maximum token = 16
    char* argv[16];
    int   argc = stringToTokens( str, argv, sizeof( argv ) / sizeof( *argv ) );

    if ( argc == 0 )
        return;

#define STRCASE( v ) upp::hash::fnv1a_32_const( v )

    switch ( upp::hash::fnv1a_32( argv[0] ) ) {
    case STRCASE( "env-report" ): {
    } break;

    case STRCASE( "ping" ): {
        API_SendHostBinary( "ping", 4 );
    } break;

    case STRCASE( "capture" ): {
        if ( argc == 1 ) {
            API_Msg( "error: command 'capture' requires argument.\n" );
            break;
        }
        AppHandler_CaptureCommand( argc - 1, argv + 1 );
    } break;

    case STRCASE( "test" ): {
        if ( argc == 1 ) {
            API_Msg( "error: command 'test' requires argument. \n" );
            break;
        }
        AppHandler_TestCommand( argc - 1, argv + 1 );
    } break;

    case STRCASE( "get" ): {
        if ( argc == 1 ) {
            API_Msgf( "error: command 'get' requires argument.\n" );
            break;
        }
        ProcessGet( argv[1] );
    } break;
    default:
        break;
    }
}

extern "C" __weak_symbol bool AppHandler_CaptureCommand( int argc,
  char*                                                      argv[] )
{
    return true;
}

extern "C" __weak_symbol bool AppHandler_CaptureBinary( char* data, size_t len )
{
    return false;
}

extern "C" __weak_symbol bool AppHandler_TestCommand( int argc, char* argv[] )
{
    return false;
}

void binaryCmdHandler( char* data, size_t len )
{
    if ( AppHandler_CaptureBinary( data, len ) ) {
        return;
    }

    API_Msg( "warning: failed to process binary data\n" );
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

        for ( *head = 0; *++head == ' '; ) {
        }

        if ( *head == 0 )
            break;

        str = head; // Non-space character pos
    }

    return num_token;
}

void apndToHostBuf( void const* d, size_t len )
{
    if ( s_hostTrBufHead + len >= sizeof(s_hostTrBuf) ) {
        return;
    }

    while ( s_bFlushing )
        vTaskDelay( 1 );

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
