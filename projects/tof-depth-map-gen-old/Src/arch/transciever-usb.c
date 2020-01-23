#include "transciever-usb.h"
#include "stm32f4xx_hal.h"
#include "usbd_cdc_if.h"
#include "../app/program.h"

void InitUsbTransceiver( struct usb_transceiver* s )
{
    s->vtable = &g_usb_transceiver_vtable;
}

transceiver_result_t vfunc_usb_transceiver_status( void* vv )
{
    return queue_buffer_size( &g_recvQueue );
}

#define min( a, b ) ( ( a ) < ( b ) ? ( a ) : ( b ) )
transceiver_result_t vfunc_usb_transceiver_write( void* s, char const* d, size_t size )
{
    // extern USBD_HandleTypeDef hUsbDeviceFS;
    while ( USBD_OK != CDC_Transmit_FS((uint8_t*) d, size) ) {}
    return size;
}
