/*! \brief SPI interface
    \file s2pi-1.h
    \author Seungwoo Kang (ki6080@gmail.com)
    \version 0.1
    \date 2019-09-14
    
    \copyright Copyright (c) 2019. Seungwoo Kang. All rights reserved.
    
 */
#pragma once
#include "main.h"

#include "platform/argus_s2pi.h"
#include "../macro.h"

EXTERNC_BEGIN

/*! \brief Indices of all available slaves that connected to SPI 1 peripheral.
    \note Values should be assigned to \ref s2pi_slave_t */
enum {
    S2PI_SLAVE_ETH    = 0,
    S2PI_SLAVE_SENSOR = 1,
    S2PI_SLAVE_MAX,
    S2PI_SLAVE_NONE = -1
};

/*! \breif      Initialize S2PI module. */
void S2PI_Init();

/*! \brief Get currently active s2pi device. 
    \returns S2PI_SLAVE_NONE if not any device is active. */
s2pi_slave_t S2PI_GetActiveDevice();

/*! \brief Synchronous version of Transfer frame function. */
void S2PI_TransferFrameSync(
    s2pi_slave_t    slave,
    uint8_t const*  txData,
    uint8_t*        rxData,
    size_t          frameSize,
    s2pi_callback_t callback,
    void*           callbackData);

EXTERNC_END