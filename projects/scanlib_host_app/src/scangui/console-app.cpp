#include "console-app.hpp"
#include <scanlib/core/scanner_protocol_handler.hpp>
#include <string>
#include <thread>
#include "app.hpp"
using namespace std::chrono_literals;

void InitConsoleApp()
{
    FScannerProtocolHandler scan;
    scan.Logger = []( auto str ) {
        printf( str );
        if ( str[strlen( str ) - 1] == '\n' )
            printf( ">> " );
    };
    scan.OnFinishScan = []( const FScanImageDesc& args ) {
        printf( ":: SAMPLES RECEVIED [ %6d ] :: \n", args.Height * args.Width );
        for ( size_t i = 0; i < args.Width * args.Height; i++ )
        {
            printf(
              "%20.17f\n", 
              args.CData()[i].Distance / double( Q9_22_ONE_INT ) );
        }
    };

    for ( ;; )
    {
        if ( API_RefreshScannerControl( scan ) == false )
        {
            printf( "Waiting for connection ... \n" );
            std::this_thread::sleep_for( 500ms );
            continue;
        }
        std::this_thread::sleep_for( 500ms );

        std::string inp;
        for ( ;; )
        {
            scan.Report();
            std::getline( std::cin, inp );

            // Evaluate connection lazily to handle disconnection during input.
            if ( scan.IsConnected() == false )
                break;

            if ( inp == "scan" )
            {
                scan.BeginCapture();
            }
            else
            {
                scan.SendString( inp.c_str() );
            }
            printf( ">> " );
        }

        printf( "Connection lost. Retrying ... \n" );
    }
}
