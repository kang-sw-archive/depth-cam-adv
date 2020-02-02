//! @file       hal.h
//! @brief      File brief description
//!
//! @author     Seungwoo Kang (ki6080@gmail.com)
//! @copyright  Copyright (c) 2019. Seungwoo Kang. All rights reserved.
//!
//! @details
//!             File detailed description
#pragma once
#include <uEmbedded/transceiver.h>
#include "dist-sensor.h"
#include "motor.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @addtogroup Depscan
//! @{
//! @addtogroup Depscan_HW
//! @{
//! @defgroup Depscan_HW_Internals
//! @{

void Internal_InitRW();

//! @}
//! @defgroup Depscan_HW_Exports
//! @{

extern motor_hnd_t              gMotX;
extern motor_hnd_t              gMotY;
extern dist_sens_t          ghDistSens;
extern transceiver_handle_t gHostConnection;

//! @}
//! @}
//! @}

#ifdef __cplusplus
}
#endif // __cplusplus
