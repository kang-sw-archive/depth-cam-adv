//!
//!
//! @author Seungwoo Kang (ki6080@gmail.com)
//! @copyright Copyright (c) 2019. Seungwoo Kang. All rights reserved.
//!
//! @details
#include <stdlib.h>
#include <uEmbedded/ring_buffer.h>
#include <uEmbedded/transceiver.h>
#include <uEmbedded/uassert.h>
#include <usbd_cdc_if.h>
#include "../defs.h"
#include "mem.h"
static transceiver_result_t cdc_read( void* desc, char* buf, size_t len );
static transceiver_result_t cdc_write( void* desc, char const* buf, size_t len );
static transceiver_result_t cdc_ioctl( void* desc, intptr_t cmd )
{
    return TRANSCEIVER_OK;
}
static transceiver_result_t cdc_close( void* desc );

static transceiver_vtable const usb_vt
    = { .read = cdc_read, .write = cdc_write, .ioctl = cdc_ioctl, .close = cdc_close };

static struct usb_rw
{
    transceiver_vtable_t const* vt_ = &usb_vt;
    ring_buffer                 rdqueue_;
    char                        rdbuf_[USB_READ_BUF_SIZE];
} s_rw;

extern "C" transceiver_handle_t OpenUsbHostConnection()
{
    uassert( s_rw.rdqueue_.buff == nullptr );
    auto ret = &s_rw;
    ring_buffer_init( &ret->rdqueue_, s_rw.rdbuf_, sizeof s_rw.rdbuf_ );
    return reinterpret_cast<transceiver_handle_t>( ret );
}

static transceiver_result_t cdc_read( void* desc, char* buf, size_t len )
{
    auto td = reinterpret_cast<usb_rw*>( desc );
    auto rd = ring_buffer_read( &td->rdqueue_, buf, len );
    return rd;
}

transceiver_result_t cdc_write( void* nouse_, char const* buf, size_t len )
{
    // Force casting.
    return CDC_Transmit_FS( (uint8_t*)buf, len ) == USBD_OK ? len : 0;
}

transceiver_result_t cdc_close( void* desc )
{
    auto td = reinterpret_cast<usb_rw*>( desc );
    AppFree( td->rdqueue_.buff );
    td->rdqueue_.buff = nullptr;
    return TRANSCEIVER_OK;
}

// Handler performs
extern "C" void CdcReceiveHandler( char* Buf, size_t len )
{
    ring_buffer_write( &s_rw.rdqueue_, Buf, len );
    USBD_CDC_SetRxBuffer( &hUsbDeviceFS, (uint8_t*)Buf );
    USBD_CDC_ReceivePacket( &hUsbDeviceFS );
}
