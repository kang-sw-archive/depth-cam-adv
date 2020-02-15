#include "communication_handler.hpp"
#include <assert.h>
#include <memory>
#include <scanlib/common/utility.hxx>

using namespace std;
using namespace std::chrono;

bool ICommunicationHandlerBase::SendBinary( char const* bin, size_t len )
{
    if ( m_os == nullptr )
        return false;

    throw bad_exception();

    lock_guard<mutex> lck( m_oslck );
    m_os->put( PACKET_BIN_OPEN_CHAR );
    m_os->write( (char const*)&PACKET_BINARY_ID, sizeof( PACKET_BINARY_ID ) );

    char             size[4];
    PACKET_SIZE_TYPE slen = len;
    upp::binutil::btoa( size, sizeof( size ), &slen, sizeof( slen ) );
    m_os->write( size, sizeof size );

    auto write = make_unique<char[]>( len * 2 );
    upp::binutil::btoa( write.get(), len * 2, bin, len );
    m_os->write( write.get(), len * 2 );
    m_os->flush();

    return true;
}

bool ICommunicationHandlerBase::SendString( char const* str )
{
    if ( m_os == nullptr )
        return false;

    lock_guard<mutex> lck( m_oslck );
    m_os->write( str, strlen( str ) );
    m_os->put( '\n' );
    m_os->flush();

    return true;
}

void ICommunicationHandlerBase::ClearConnection() noexcept
{
    lock_guard lck { m_shutdown_lock };
    m_strmbuf = nullptr;
    m_os      = nullptr;
}

void ICommunicationHandlerBase::OnBinaryData( char const* data, size_t len )
{
    printf( "Received %zu bytes of data. \n", len );
}

ICommunicationHandlerBase::EPacketProcessResult
ICommunicationHandlerBase::ProcessSinglePacket( size_t TimeoutMs )
{
    lock_guard lck { m_shutdown_lock };
    using chrono::system_clock;
    auto       WaitBeginTime = chrono::system_clock::now();
    auto const buf           = m_buff.get();
    auto const strm          = m_strmbuf.get();
    size_t     rd            = 0;
    auto const timeout       = milliseconds( TimeoutMs );

    if ( !( buf && strm ) )
        return EPacketProcessResult::PACKET_ERROR_DISCONNECTED;

    char*       head = buf;
    char const* end  = buf + m_buffSize;

    auto const WaitDuration = [&WaitBeginTime]() {
        return duration_cast<milliseconds>(
                 system_clock::now() - WaitBeginTime )
          .count();
    };

    for ( ;; )
    {
        if ( strm->sgetn( head, 1 ) == 0 )
        {
            if ( WaitDuration() > TimeoutMs )
            {
                // On timeout, flush current string
                if ( buf != head )
                {
                    *head = 0;
                    OnString( buf );
                }
                return EPacketProcessResult::PACKET_ERROR_TIMEOUT;
            }
            continue;
        }

        WaitBeginTime = system_clock::now();
        char ch       = *head++;

        if ( ch == '\0' || ch == '\n' || head == end )
        {
            *head = 0;
            OnString( buf );
            head = buf;
            continue;
        }

        // When binary data incoming...
        if ( ch == PACKET_BIN_OPEN_CHAR )
        {
            head = buf;

            for ( std::streamsize result;
                  ( result = strm->sgetn( head, 1 ),
                    *head != PACKET_BIN_CLOSE_CHAR );
                  ++head )
            {
                if ( result )
                {
                    WaitBeginTime = system_clock::now();
                    continue;
                }

                if ( WaitDuration() > TimeoutMs )
                {
                    return EPacketProcessResult::PACKET_ERROR_TIMEOUT;
                }
            }

            auto Length = ( head - buf ) / 2;
            if ( !upp::binutil::atob( buf, buf, Length ) )
            {
                return PACKET_ERROR_INVALID_HEADER;
            }

            OnBinaryData( buf, Length );
            break;
        }
    } // End of loop

    return EPacketProcessResult::PACKET_OK;
}

void ICommunicationHandlerBase::InitializeStream(
  unique_ptr<streambuf> strm,
  size_t                recvSz )
{
    assert( strm && recvSz );
    m_strmbuf  = std::move( strm );
    m_os       = make_unique<ostream>( m_strmbuf.get(), false );
    m_buff     = make_unique<char[]>( recvSz );
    m_buffSize = recvSz;
}