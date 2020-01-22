#include "com.hpp"
#include <Windows.h>

comstreambuf_t::comstreambuf_t( char const* com_port ) : m_hCom( NULL ),
                                                         ibuf()
{
    char buf[128];
    sprintf_s( buf, "\\\\.\\%s", com_port );

    // Create file object for opening COM port
    auto hComm = CreateFile(
        buf,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_WRITE_THROUGH,
        NULL );

    if ( hComm == INVALID_HANDLE_VALUE )
        return;

    m_hCom = hComm;

    DCB initdcb;

    get_dcb( &initdcb );
    initdcb.fInX    = 0;
    initdcb.fOutX   = 0;
    initdcb.fNull   = 0;
    initdcb.fBinary = 1;
    set_dcb( &initdcb );

    setg( ibuf, ibuf + 1, ibuf + 1 );
}

comstreambuf_t::~comstreambuf_t()
{
    if ( *this ) {
        CloseHandle( m_hCom );
    }
}

comstreambuf_t::operator bool() const
{
    return m_hCom;
}

bool comstreambuf_t::get_dcb( _DCB* dcbOut ) const
{
    return m_hCom && GetCommState( m_hCom, dcbOut );
}

bool comstreambuf_t::set_dcb( _DCB* dcbIn )
{
    return m_hCom && SetCommState( m_hCom, ( dcbIn ) );
}

void comstreambuf_t::set_timeout( _COMMTIMEOUTS* timeout )
{
    SetCommTimeouts( m_hCom, timeout );
}

comstreambuf_t::strmbuf_t::int_type comstreambuf_t::overflow( strmbuf_t::int_type c )
{
    DWORD written;
    return *this && WriteFile( m_hCom, &c, 1, &written, NULL ) && written;
}

int comstreambuf_t::sync()
{
    // Flushing operation ... not meaningful for now.
    return 0;
}

comstreambuf_t::strmbuf_t::int_type comstreambuf_t::underflow()
{
    char                c;
    DWORD               readcnt = 0;
    strmbuf_t::int_type retval;
    bool                stat;
    do {
        stat  = ReadFile( m_hCom, &c, sizeof( c ), &readcnt, NULL );
        *ibuf = c;

        if ( readcnt ) {
            retval = traits_type::not_eof( c );
            setg( ibuf, ibuf, ibuf + 1 );
            return retval;
        }
    } while ( stat );

    return traits_type::eof();
}

std::streamsize comstreambuf_t::xsputn( const char* _Ptr, std::streamsize _Count )
{
    DWORD written = 0;
    return ( *this && WriteFile( m_hCom, _Ptr, static_cast<DWORD>( _Count ), &written, NULL ) ) ? written : 0;
}

std::streamsize comstreambuf_t::xsgetn( char* _Ptr, std::streamsize _Count )
{
    if ( *this == false )
        return 0;

    DWORD           readcnt = 0;
    std::streamsize cnt     = 0;
    bool            stat;
    do {
        stat = ReadFile( m_hCom, _Ptr, static_cast<DWORD>( _Count ), &readcnt, NULL );
        cnt += readcnt;
        _Ptr += readcnt;
        // printf("readcnt: %d of %d\n", readcnt, _Count);
    } while ( stat && readcnt && cnt < _Count );

    return readcnt;
}
