#include "app.h"

//! @todo. Implement log protocol wrapper using gHostConnection()

/////////////////////////////////////////////////////////////////////////////
// .vtable decl
static transceiver_result_t log_write( void*, char const*, size_t );

static transceiver_vtable_t s_vt = {
    .read  = nullptr,
    .ioctl = nullptr,
    .close = nullptr,
    .write = log_write };

/////////////////////////////////////////////////////////////////////////////
// Log struct
static struct {
    transceiver_vtable_t* vt_ = &s_vt;

} s_log;

/////////////////////////////////////////////////////////////////////////////
// Log layer def
transceiver_handle_t gLog = reinterpret_cast<transceiver_handle_t>( &s_log );

/////////////////////////////////////////////////////////////////////////////
// Impl
transceiver_result_t log_write( void*, char const*, size_t )
{
    return TRANSCEIVER_OK;
}
