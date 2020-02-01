#include <platform/argus_s2pi.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//! @addtogroup Depscan
//! @{
//! @defgroup Depscan_HW
//! @{
//! @defgroup Depscan_HW_S2PI
//! @{

// Slave index type
typedef enum
{
    S2PI_SLAVE_NONE,
    S2PI_SLAVE_ARGUS,
    S2PI_SLAVE_MAX,
} S2PI_SLAVE_t;

//! Synchronously send and receive data via S2PI hardware.
//! @param slave: Predefined integer constant index to specify S2PI slave.
//! @param txData: Data pointer to transmit. Necessary.
//! @param rxData: Data pointer to receive. Optional.
//! @param frameSize: Size of Tx/Rx request. Buffer size must be larger than this value.
//! @param callback: Called when transfer is finished.
//! @param callbackData: Additional argument which will be passed into callback.
void S2PI_TransferFrameSync(
    s2pi_slave_t    slave,
    uint8_t const*  txData,
    uint8_t*        rxData,
    size_t          frameSize,
    s2pi_callback_t callback,
    void*           callbackData );

//! @}
//! @}
//! @}
#ifdef __cplusplus
}
#endif // __cplusplus
