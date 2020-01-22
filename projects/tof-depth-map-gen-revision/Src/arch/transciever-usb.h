#pragma once
#include "uEmbedded/queue_allocator.h"
#include "uEmbedded/transceiver.h"

/*! \brief      An implementation class of usb transceiver, which helps encapsulization of overall process.
    \return     */
struct usb_transceiver {
    //! \brief      Reference for vtable. Must be placed on the first of all elements !
    transceiver_vptr_t vtable;
};

/*! \brief      Initialize usb transceiver.
    \param s */
void InitUsbTransceiver(struct usb_transceiver* s);

//! \breif      Do nothing since usb device is assumed open always.
static transceiver_result_t vfunc_usb_transceiver_open(void* placeholder) { return 0; }

//! \breif      Do nothing since usb device is assumed open always.
static transceiver_result_t vfunc_usb_transceiver_close(void* placeholder) { return 0; }

static transceiver_result_t vfunc_usb_transceiver_read(void* ph__, char* ph__1, size_t ph__2) { return 0; }

//! \breif
transceiver_result_t vfunc_usb_transceiver_status(void* vv);

//! \breif
transceiver_result_t vfunc_usb_transceiver_write(void*, char const*, size_t);

static transceiver_vtable_t g_usb_transceiver_vtable = {
    .open   = vfunc_usb_transceiver_open,
    .close  = vfunc_usb_transceiver_close,
    .status = vfunc_usb_transceiver_status,
    .read   = vfunc_usb_transceiver_read,
    .write  = vfunc_usb_transceiver_write};
