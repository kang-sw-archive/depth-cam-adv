#include "hw.h"
#include <usb_device.h>
#include <usbd_cdc_if.h>
#include "../app/hal.h"
#include "../defs.h"
/////////////////////////////////////////////////////////////////////////////
// Globals
transceiver_handle_t gHostConnection;

/////////////////////////////////////////////////////////////////////////////
// Decls
transceiver_handle_t OpenUsbHostConnection();

/////////////////////////////////////////////////////////////////////////////
// Defs
void Internal_InitRW()
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
    void S2PI_Init();
    S2PI_Init();
    void InitMotors();
    InitMotors();
}
