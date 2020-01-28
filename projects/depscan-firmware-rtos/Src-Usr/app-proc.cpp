#include "app.h"
#include "defs.h"
#include "protocol.h"
#include "rw.h"
#include <semphr.h>
#include <stdlib.h>

/////////////////////////////////////////////////////////////////////////////
// Host IO communication handler
//
//

/////////////////////////////////////////////////////////////////////////////
// Static utilities

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
        char buf[PACKET_LENGTH( packet ) + 1];
        if ( readHostConn( buf, PACKET_LENGTH( packet ) ) == false )
            continue;


    }
}

extern "C" void AppTask_CmdProc( void* )
{
}

void API_SendToHost( void const* data, size_t len )
{
}

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
