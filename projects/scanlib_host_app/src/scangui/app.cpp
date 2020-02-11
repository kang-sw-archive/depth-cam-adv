//! Entry point of scanner GUI application
//!
//! @author Seungwoo Kang (ki6080@gmail.com)
//! @copyright Copyright (c) 2019. Seungwoo Kang. All rights reserved.
//!
//! @details
//!
#include "app.hpp"
#include "console-app.hpp"
#include <gflags/gflags.h>
#include <memory>
#include <thread>

using namespace std;

//
// Flag definitions
//
DEFINE_bool( no_gui, false, "Disables GUI mode" );
DEFINE_bool( show_console, false, "Hide console window printing log" );
DEFINE_string( com, "", "Specify com port name manually. e.g. COM5" );
DEFINE_string( config_path, "", "Specify configuration path" );

int gui_app( int argc, char** argv );
int gui_view_app( int argc, char** argv );
int main( int argc, char** argv )
{
    gflags::ParseCommandLineFlags( &argc, &argv, true );

    // If any argument is given, window will open as viewer mode.
    if ( argc >= 2 ) {
        API_SystemShowConsole( false );
        return gui_view_app( argc, argv );
    }
    else {
        if ( FLAGS_no_gui ) {
            //! @todo. Go to console mode ...
            API_SystemShowConsole( true );
            InitConsoleApp();
        }
        else {
            API_SystemShowConsole( FLAGS_show_console );
            return gui_app( argc, argv );
        }
    }

    return 0;
}

bool API_RefreshScannerControl( FScannerProtocolHandler& S )
{
    char Port[20];
    Port[0] = 0;
    API_FindConnection( Port );
    FLAGS_com = Port;

    if ( FLAGS_com.empty() ) {
        return false;
    }
    API_SystemCreateScannerControl( S );
    return true;
}