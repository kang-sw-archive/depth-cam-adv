#pragma once
#include <uEmbedded/transceiver.h>

#ifdef __cplusplus
extern "C" {
#endif

//! Connection to host.
extern transceiver_descriptor_t gHostConnection;

//! Initailize all architecture associated read-write operation.
void InitRW();

#ifdef __cplusplus
}
#endif // __cplusplus
