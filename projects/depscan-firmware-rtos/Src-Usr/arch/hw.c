#include "hw.h"
#include "../defs.h"
#include "../rw.h"
#include <usb_device.h>
#include <usbd_cdc_if.h>

/////////////////////////////////////////////////////////////////////////////
// Globals
transceiver_handle_t gHostConnection;

/////////////////////////////////////////////////////////////////////////////
// Decls
transceiver_handle_t OpenUsbHostConnection();

/////////////////////////////////////////////////////////////////////////////
// Defs
void InitRW()
{
    gHostConnection = OpenUsbHostConnection();
}

bool HW_CheckUsbConnection()
{
    //! @todo. Implement VBUS detection
    return false;
}

void InitHW()
{
    MX_USB_DEVICE_Init();
    HW_TIMER_INIT();
}
