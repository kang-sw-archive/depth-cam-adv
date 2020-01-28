#include "hw.h"
#include <usb_device.h>
#include <usbd_cdc_if.h>

bool HW_CheckUsbConnection()
{
    //! @todo. Implement VBUS detection
    return false;
}

void InitHW()
{
    MX_USB_DEVICE_Init();
}
