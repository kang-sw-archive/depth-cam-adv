#pragma once
#include <functional>
#include <tuple>
#include "../core/scanner_protocol_handler.hpp"

#define SICO_VENDOR_ID          "6080"
#define SICO_SCANNER_PRODUCT_ID "ccd2"

void API_SystemShowConsole( bool bShow );
void API_SystemCreateScannerControl(
  FScannerProtocolHandler&,
  char const* COMSTR );
void API_FindConnection( char* PortName );
bool API_RefreshScannerControl( FScannerProtocolHandler& );
bool API_MappToRGB(
  intptr_t                     context,
  std::function<void( void* )> draw_cb,
  void*                        buffer );
