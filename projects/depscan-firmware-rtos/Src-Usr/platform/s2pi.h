#include <platform/argus_s2pi.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// Slave index type
typedef enum {
    S2PI_SLAVE_NONE,
    S2PI_SLAVE_ARGUS,
    S2PI_SLAVE_MAX,
} S2PI_SLAVE_t;

void S2PI_TransferFrameSync(
    s2pi_slave_t    slave,
    uint8_t const*  txData,
    uint8_t*        rxData,
    size_t          frameSize,
    s2pi_callback_t callback,
    void*           callbackData );

#ifdef __cplusplus
}
#endif // __cplusplus
