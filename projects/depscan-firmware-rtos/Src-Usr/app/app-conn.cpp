#include <FreeRTOS.h>

#include <alloca.h>
#include <main.h>
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
static char         s_hostTrBuf[HOST_TRANSFER_BUFFER_SIZE];
static size_t       s_hostTrBufHead = 0;
static volatile int s_writingTask   = 0;
static void         flushTransmitData();
TaskHandle_t        s_hTask;

// Read host connection for requested byte length.
// @returns false when failed to receive data, with given timeout.
static bool readHostConn( void* dst, size_t len );

#define IS_IRQ() ( __get_PRIMASK() != 0 || __get_IPSR() != 0 )

/////////////////////////////////////////////////////////////////////////////
// Primary Procedure
extern "C" _Noreturn void AppProc_HostIO( void* nouse_ )
{
    static char buf[HOST_RECEIVE_BUFFER_SIZE];
    char*       head = buf;

    for ( ;; )
    {
        // Read data byte by byte
        if ( readHostConn( head, 1 ) == false )
            continue;

        char ch = *head++;

        // Handle string command
        if ( ch == '\n' || ch == '\r' )
        {
            size_t len = head - buf - 1;
            head[-1]   = '\0';
            head       = buf;
            if ( len )
            {
                stringCmdHandler( buf, len );
            }
            continue;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
// Global function defs
extern "C" {
static void OpenChar()
{
    s_hostTrBuf[s_hostTrBufHead++] = PACKET_BIN_OPEN_CHAR;
}

static void CloseChar()
{
    s_hostTrBuf[s_hostTrBufHead++] = PACKET_BIN_CLOSE_CHAR;
}

static void AppendBinaryRaw( void const* data, size_t len )
{
    portENTER_CRITICAL();
    auto at = s_hostTrBuf + s_hostTrBufHead;
    s_hostTrBufHead += len * 2;
    uassert( s_hostTrBufHead < sizeof( s_hostTrBuf ) );
    auto wbyte = upp::binutil::btoa( at, len * 2, data, len );
    uassert( wbyte == len );
    portEXIT_CRITICAL();
}

void API_SendHostBinary( void const* data, size_t len )
{
    portENTER_CRITICAL();
    OpenChar();
    AppendBinaryRaw( data, len );
    CloseChar();
    portEXIT_CRITICAL();
}

void API_SendHostBinaries(
  void const* const data[],
  size_t const      len[],
  size_t            cnt )
{
    PACKET_SIZE_TYPE sum = 0;
    for ( size_t i = 0; i < cnt; i++ )
        sum += len[i];

    portENTER_CRITICAL();
    OpenChar();
    for ( size_t i = 0; i < cnt; i++ )
        AppendBinaryRaw( data[i], len[i] );
    CloseChar();
    portEXIT_CRITICAL();
}

void API_SendHostString( void const* data, size_t len )
{
    API_SendHostRaw( data, len );
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

    if ( head->id_ != id )
    {
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
    if ( arr[idx].id_ == id )
    {
        //! @todo. Implement array_remove from uEmbedded ...
    }
}
}

static void GetHandler( char const* name )
{
    // Find ref
    auto id       = upp::hash::fnv1a_32( name );
    using ed      = export_data::node;
    ed* const arr = s_xd.node_;

    auto idx
      = lowerbound( arr, &id, sizeof( ed ), s_xd.size_, export_data::compare );
    auto at = arr + idx;

    if ( at->id_ != id )
    {
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

static void vprint__( char const* fmt, va_list vp );

extern "C" void API_Putf( char const* fmt, ... )
{
    va_list vp;
    va_start( vp, fmt );
    vprint__( fmt, vp );
    va_end( vp );
}

void API_Msgf( char const* fmt, ... )
{
    va_list vp;
    va_start( vp, fmt );
    taskENTER_CRITICAL();
    API_Msg( "" );
    vprint__( fmt, vp );
    taskEXIT_CRITICAL();
    va_end( vp );
}

extern "C" int API_Msg( char const* txt )
{
    auto t = API_GetTime_us();
    char buf[17];
    sprintf(
      buf,
      "[%6u.%06u] ",
      ( uint32_t )( t / 1000000u ),
      ( uint32_t )( t % 1000000u ) );
    taskENTER_CRITICAL();
    API_SendHostString( buf, sizeof( buf ) );
    API_SendHostString( txt, strlen( txt ) + 1 );
    taskEXIT_CRITICAL();
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
// Utility defs
bool readHostConn( void* dst, size_t len )
{
    size_t numRetries = 5;
    for ( ; numRetries; )
    {
        // Flush transmit data before try read
        flushTransmitData();

        auto result = td_read( gHostConnection, (char*)dst, len );

        // If there's nothing to read, sleep for 1 cycle
        if ( result == 0 )
        {
            vTaskDelay( 1 );
            continue;
        }
        else if ( result < 0 )
        {
            vTaskDelay( pdMS_TO_TICKS( 10 ) );
            --numRetries;
        }

        len -= result;
        dst = (char*)dst + result;

        if ( len == 0 )
        {
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

    switch ( upp::hash::fnv1a_32( argv[0] ) )
    {
    case STRCASE( "env-report" ):
    {
    }
    break;

    case STRCASE( "ping" ):
    {
        API_SendHostBinary( "ping", 4 );
    }
    break;

    case STRCASE( "capture" ):
    {
        AppHandler_CaptureCommand( argc - 1, argv + 1 );
    }
    break;

    case STRCASE( "test" ):
    {
        AppHandler_TestCommand( argc - 1, argv + 1 );
    }
    break;

    case STRCASE( "get" ):
    {
        if ( argc == 1 )
        {
            API_Msgf( "error: command 'get' requires argument.\n" );
            break;
        }
        GetHandler( argv[1] );
    }
    break;
    default:
        break;
    }
}

extern "C" __weak_symbol bool
AppHandler_CaptureCommand( int argc, char* argv[] )
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
    if ( AppHandler_CaptureBinary( data, len ) )
    {
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

    for ( ; num_token < (int)argv_len; )
    {
        if ( *head != ' ' && *head != '\0' )
        {
            ++head;
            continue;
        }

        argv[num_token++] = str;
        if ( *head == 0 )
            break;

        for ( *head = 0; *++head == ' '; )
        {
        }

        if ( *head == 0 )
            break;

        str = head; // Non-space character pos
    }

    return num_token;
}

void API_SendHostRaw( void const* d, size_t len )
{
    uassert( s_hostTrBufHead + len < sizeof( s_hostTrBuf ) );

    taskENTER_CRITICAL();
    s_hostTrBufHead += len;
    memcpy( s_hostTrBuf + s_hostTrBufHead - len, d, len );
    taskEXIT_CRITICAL();
}

static void vprint__( char const* fmt, va_list vp )
{
    va_list vp2;
    va_copy( vp2, vp );
    size_t allocsz = vsnprintf( NULL, 0, fmt, vp ) + 1;

    uassert( s_hostTrBufHead + allocsz < sizeof( s_hostTrBuf ) );

    taskENTER_CRITICAL();
    char* buf = s_hostTrBuf + s_hostTrBufHead;
    s_hostTrBufHead += allocsz;
    vsprintf( buf, fmt, vp2 );
    taskEXIT_CRITICAL();
    va_end( vp2 );
}

void flushTransmitData()
{
    if ( s_hostTrBufHead == 0 )
        return;

    taskENTER_CRITICAL();
    td_write( gHostConnection, s_hostTrBuf, s_hostTrBufHead );
    s_hostTrBufHead = 0;
    taskEXIT_CRITICAL();
}
