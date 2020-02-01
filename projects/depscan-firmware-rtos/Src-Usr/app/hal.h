//! \file       hal.h
//! \brief      File brief description
//!
//! \author     Seungwoo Kang (ki6080@gmail.com)
//! \copyright  Copyright (c) 2019. Seungwoo Kang. All rights reserved.
//!
//! \details
//!             File detailed description
#pragma once
#include <uEmbedded/transceiver.h>
#ifdef __cplusplus
extern "C" {
#endif

/////////////////////////////////////////////////////////////////////////////
// Decls
//! Connection to host.
//! This is to adapt various IO devices via the unity interface.
extern transceiver_handle_t gHostConnection;

/////////////////////////////////////////////////////////////////////////////
// Functions
//! Initailize all architecture associated to read-write operation.
void InitRW();

extern struct dist_sens__* ghDistSens;

#ifdef __cplusplus
}
#endif // __cplusplus
