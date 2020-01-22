#pragma once
#include <functional>
#include <scanlib/core/scanner_protocol_handler.hpp>
#include <tuple>

void API_SystemShowConsole( bool bShow );
void API_SystemCreateScannerControl( FScannerProtocolHandler& );
void API_FindConnection( char* PortName );
bool API_RefreshScannerControl( FScannerProtocolHandler& );
bool API_MappToRGB( intptr_t context, std::function<void( void* )> draw_cb, void* buffer );
