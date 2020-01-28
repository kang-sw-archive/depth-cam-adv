#include "app.h"
#include "defs.h"
#include "protocol.h"
#include "rw.h"
#include <semphr.h>
#include <stdlib.h>
#include <uEmbedded-pp/utility.hxx>

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
static int stringToTokens( char* str, char const* argv[], size_t argv_len );

// Flush buffered data to host
static void flushTransmitData();

// Read host connection for requested byte length.
// @returns false when failed to receive data, with given timeout.
static bool readHostConn( void* dst, size_t len );

/////////////////////////////////////////////////////////////////////////////
// Primary Procedure
extern "C" void AppTask_HostIO( void* nouse_ )
{
    packetinfo_t packet;

    for ( ;; ) {
        // Check if read data has valid protocol.
        if ( readHostConn( &packet, PACKET_SIZE ) == false )
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
void API_SendToHost( void const* data, size_t len )
{
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

#define STRCASE( v ) upp::hash::fnv1a_32( v )
    uint32_t cmdidx = STRCASE( argv[0] );

    switch ( cmdidx ) {
    case STRCASE( "app-os-report" ):
        break;

    default:
        App_HandleCaptureCommand();
        break;
    }
}

__weak_symbol bool App_HandleCaptureCommand()
{
    return false;
}

void binaryCmdHandler( char* data, size_t len )
{
}