#include <Windows.h>
#include <cassert>
#include <conio.h>
#include <future>
#include <scanlib/arch/win32/com.hpp>
#include <scanlib/core/scanner_protocol_handler.hpp>
#include <string>
#include <string_view>
#include <thread>
using namespace std;

int main( void )
{
    FScannerProtocolHandler S;
    S.Logger       = []( const char* str ) { printf( str ); };
    auto ComOpener = []( FScannerProtocolHandler& ) -> unique_ptr<comstreambuf_t> {
        auto         ret = make_unique<comstreambuf_t>( "COM3" );
        COMMTIMEOUTS to  = { MAXDWORD, 1, 1, 1, 1 };
        ret->set_timeout( &to );

        if ( *ret == false )
            return nullptr;
        return move( ret );
    };

    S.OnFinishScan                         = []( auto img ) { printf( "Image scanning done!\n" ); };
    FCommunicationProcedureInitStruct init = {};
    init.ConnectionRetryCount              = (size_t)-2;
    init.TimeoutMs                         = 5000;
    auto retval                            = S.Activate( ComOpener, init );
    printf( "Activate result: %d", retval );

    char buf[1024];

    FScannerProtocolHandler::CaptureParam param;
    param.DesiredAngle.emplace( 45.f, 35.f );
    param.DesiredResolution.emplace( 22, 15 );
    param.CaptureDelayUs = 1000;

    for ( ;; ) {
        fputs( ">> ", stdout );
        fgets( buf, sizeof( buf ), stdin );
        string str = buf;
        str.pop_back();

        if ( str == string_view( "start" ) ) {
            printf( "Beginning capture ...\n" );
            S.BeginCapture( &param );
        }
        else if ( str == "stop" ) {
            printf( "Stopping capture ...\n" );
            S.StopCapture();
        }
        else if ( str == string_view( "q" ) ) {
            break;
        }
        else {
            ( (ICommunicationHandlerBase*)&S )->SendString( str.c_str() );
        }
    }

    return 0;
}