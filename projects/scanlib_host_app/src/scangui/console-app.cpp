#include "console-app.hpp"
#include "app.hpp"
#include <scanlib/core/scanner_protocol_handler.hpp>
#include <thread>
using namespace std::chrono_literals;

void InitConsoleApp()
{
    FScannerProtocolHandler scan;
    char                    buf[2048];
    scan.Logger = []( auto str ) { printf(str); if(str[strlen(str)-1] =='\n') printf(">> "); };

    for ( ;; ) {
        if ( API_RefreshScannerControl( scan ) == false ) {
            printf( "Waiting for connection ... \n" );
            std::this_thread::sleep_for( 500ms );
            continue;
        }
        std::this_thread::sleep_for( 500ms );

        for ( ;; ) {
            fgets( buf, sizeof( buf ), stdin );
            buf[strlen( buf ) - 1] = '\0';

            // Evaluate connection lazily to handle disconnection during input.
            if ( scan.IsConnected() == false )
                break;

            scan.SendString( buf );
            printf( ">> " );
        }

        printf( "Connection lost. Retrying ... \n" );
    }
}
