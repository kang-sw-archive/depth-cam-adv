#include "communication_handler.hpp"
#include <assert.h>

using namespace std;
using namespace std::chrono;

bool ICommunicationHandlerBase::SendBinary( char const* bin, size_t len )
{
    if ( m_os == nullptr )
        return false;

    lock_guard<mutex> lck( m_oslck );
    packetinfo_t      header = PACKET_MAKE( false, len );
    m_os->write( (char*)&header, sizeof( packetinfo_t ) );
    m_os->write( bin, len );
    m_os->flush();

    return true;
}

bool ICommunicationHandlerBase::SendString( char const* str )
{
    if ( m_os == nullptr )
        return false;

    lock_guard<mutex> lck( m_oslck );
    auto              len    = strlen( str ) + 1;
    packetinfo_t      header = PACKET_MAKE( true, len );
    m_os->write( (char*)&header, sizeof( packetinfo_t ) );
    m_os->write( str, len );
    m_os->flush();

    return true;
}

void ICommunicationHandlerBase::OnBinaryData( char const* data, size_t len )
{
    printf( "Received %zu bytes of data. \n", len );
}

ICommunicationHandlerBase::EPacketProcessResult ICommunicationHandlerBase::ProcessSinglePacket( size_t TimeoutMs )
{
    auto         WaitBeginTime = chrono::system_clock::now();
    milliseconds WaitDuration  = {};
    auto const   buff          = m_buff.get();
    auto const   strm          = m_strmbuf.get();
    size_t       rd            = 0;
    auto const   timeout       = milliseconds( TimeoutMs );

    if ( !( buff && strm ) )
        return EPacketProcessResult::PACKET_ERROR_DISCONNECTED;

    // Read header.
    while ( ( rd += strm->sgetn( buff, sizeof( packetinfo_t ) - rd ) ) < sizeof( packetinfo_t ) ) {
        WaitDuration = duration_cast<milliseconds>( system_clock::now() - WaitBeginTime );
        if ( WaitDuration > timeout )
            return EPacketProcessResult::PACKET_ERROR_TIMEOUT;
        continue;
    }

    // Parse header
    auto info = *(packetinfo_t const*)( buff );

    // Verify header
    if ( PACKET_IS_PACKET( info ) == false && InvalidHeaderException( &info ) == false ) {
        return EPacketProcessResult::PACKET_ERROR_INVALID_HEADER;
    }

    // Receive data
    {
        char*  head = buff;
        size_t len  = PACKET_LENGTH( info );
        while ( len ) {
            size_t readcnt = strm->sgetn( head, len );

            if ( readcnt ) // Reset wait begin time when any data is read.
                WaitBeginTime = system_clock::now();
            WaitDuration = duration_cast<milliseconds>( system_clock::now() - WaitBeginTime );
            if ( WaitDuration > timeout )
                return EPacketProcessResult::PACKET_ERROR_TIMEOUT;

            head += readcnt;
            len -= readcnt;
        }

        // Handle received data.
        if ( PACKET_IS_STR( info ) ) {
            // Append null character to prevent exception.
            *head = '\0';
            OnString( buff );
        }
        else {
            OnBinaryData( buff, PACKET_LENGTH( info ) );
        }
    }

    return EPacketProcessResult::PACKET_OK;
}

void ICommunicationHandlerBase::InitializeStream( unique_ptr<streambuf> strm, size_t recvSz )
{
    assert( strm && recvSz );
    m_strmbuf = std::move( strm );
    m_os      = make_unique<ostream>( m_strmbuf.get(), false );
    m_buff    = make_unique<char[]>( recvSz );
}