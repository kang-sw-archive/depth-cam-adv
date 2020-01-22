/*! \brief 
    \file s2pi-1.c
    \author Seungwoo Kang (ki6080@gmail.com)
    \version 0.1
    \date 2019-09-14
    
    \copyright Copyright (c) 2019. Seungwoo Kang. All rights reserved.
    
 */
#include "api/argus_def.h"

#include "s2pi-1.h"

#include <uEmbedded/uassert.h>

#include "stm32f4xx_hal.h"
#include <app/program.h>
#include <arch/hw-timer.h>
#include <arch/irq.h>
#include <platform/argus_timer.h>
#include <stdbool.h>
#include <uEmbedded/event-procedure.h>

struct slave_desc {
    // For each slaves ...
    GPIO_TypeDef* chipSelectPort;
    //
    uint16_t chipSelectPin;
    // Default = low activ
    uint8_t csActiveVal;
    // Callback
    s2pi_callback_t callback;
    // Callback parameter
    void* callbackParam;
    // Status cache
    status_t latestTransferStatus;
};

#define ASYNC_SPI 0
#define print(...) // for(volatile size_t i = 155; --i; )

// All slaves
__IO struct slave_desc g_slaves[S2PI_SLAVE_MAX] = {
    // ETHERNET SPI
    {.chipSelectPort       = NULL,
     .csActiveVal          = 0,
     .callback             = NULL,
     .callbackParam        = NULL,
     .chipSelectPin        = GPIO_PIN_2,
     .latestTransferStatus = STATUS_OK},
    // SENSOR SPI
    {.chipSelectPort       = GPIOC,
     .csActiveVal          = 0,
     .callback             = NULL,
     .callbackParam        = NULL,
     .chipSelectPin        = GPIO_PIN_0,
     .latestTransferStatus = STATUS_OK},
};

// Indicates the slave which occupying the spi device.
static __IO s2pi_slave_t g_activeSlave = S2PI_SLAVE_NONE;

// Indicates whether the rx / tx operation has done.
// Runs as semaphore.
__IO uint8_t g_rxtxRunning = 0;

// Checks if SPI device is currently in GPIO mode.
__IO bool g_isGpioMode = false;

/* <>---------------------------------------------------------------->
   *
   * TRANSFER FUNCTIONS
   *
   <----------------------------------------------------------------<> */

// On transfer error
//void HAL_SPI_ErrorCallback( SPI_HandleTypeDef* hspi )
//{
//    if ( g_rxtxRunning > 0 )
//    {
//        --g_rxtxRunning;
//    }
//}
//
//void HAL_SPI_TxCpltCallback( SPI_HandleTypeDef* nouse_ )
//{
//    if ( g_rxtxRunning > 0 )
//    {
//        --g_rxtxRunning;
//    }
//}
//
//void HAL_SPI_RxCpltCallback( SPI_HandleTypeDef* nouse_ )
//{
//    if ( g_rxtxRunning > 0 )
//    {
//        --g_rxtxRunning;
//    }
//}
//
//void HAL_SPI_TxRxCpltCallback( SPI_HandleTypeDef* nouse_ )
//{
//    if ( g_rxtxRunning > 0 )
//    {
//        --g_rxtxRunning;
//    }
//}
//
//void HAL_SPI_AbortCpltCallback( SPI_HandleTypeDef* hspi )
//{
//    transfer_done( ERROR_ABORTED );
//}
static void timer_cb__spi(void* p)
{
    struct slave_desc* s = (struct slave_desc*)p;
    s->callback(s->latestTransferStatus, s->callbackParam);
}

void transfer_done(int val)
{
    if (g_rxtxRunning > 0) {
        g_rxtxRunning--;
    }

    // LogPrintf( "Transfer done called for %d. \n", g_rxtxRunning );
    if (g_rxtxRunning == 0) {
        struct slave_desc* s = g_slaves + g_activeSlave;
        HAL_GPIO_WritePin(s->chipSelectPort, s->chipSelectPin, !s->csActiveVal);

        // print("Receiving Bytes ...\n");
        if (s->callback) {
            // logputs("Calling callback\n");
            // s->callback( val, s->callbackParam );
            QueueTimer(timer_cb__spi, s, 15);
        }
        // print("Callback done.\n");
    }

    // Put initial value as high on IRQ pin.
    // HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
}

static void S2PI_IRQ_Callback();

void S2PI_Init()
{
    EXTI_CALLBACK_LINE[1] = S2PI_IRQ_Callback;

    for (size_t i = 0; i < S2PI_SLAVE_MAX; i++) {
        struct slave_desc* s = g_slaves + i;
        HAL_GPIO_WritePin(s->chipSelectPort, s->chipSelectPin, !s->csActiveVal);
    }
}

void S2PI_TransferFrameSync(s2pi_slave_t slave, uint8_t const* txData, uint8_t* rxData, size_t frameSize, s2pi_callback_t callback, void* callbackData)
{
    while (S2PI_TransferFrame(slave, txData, rxData, frameSize, callback, callbackData) != STATUS_OK) {}
    while (S2PI_GetStatus() != STATUS_IDLE) {}
}

// Some random number.
uint32_t S2PI_GetBaudRate(void)
{
    return (uint32_t)12e6;
}

status_t S2PI_SetBaudRate(uint32_t baudRate_Bps)
{
    // No effect
    return STATUS_OK;
}

status_t S2PI_GetStatus(void)
{
    if (g_rxtxRunning) {
        return STATUS_BUSY;
    }
    else if (g_isGpioMode) {
        return STATUS_S2PI_GPIO_MODE;
    }
    else {
        return STATUS_IDLE;
    }
}
 

status_t S2PI_TransferFrame(s2pi_slave_t    slave,
                            uint8_t const*  txData,
                            uint8_t*        rxData,
                            size_t          frameSize,
                            s2pi_callback_t callback,
                            void*           callbackData)
{
    uassert(txData);
    uassert(frameSize);

    // print("T:%db \n", frameSize);

    status_t stat = S2PI_GetStatus();

    if (slave >= S2PI_SLAVE_MAX)
        return ERROR_S2PI_INVALID_SLAVE;
    if (stat != STATUS_IDLE)
        return stat;

    irq_lock();

    g_activeSlave        = slave;
    struct slave_desc* s = &g_slaves[g_activeSlave];
    s->callback          = callback;
    s->callbackParam     = callbackData;

    // Select slave. Deselect happens when transmission done.
    HAL_GPIO_WritePin(s->chipSelectPort, s->chipSelectPin, s->csActiveVal);

    HAL_StatusTypeDef res;
    status_t          retval;

    if (rxData) {
#if !ASYNC_SPI
        res = HAL_SPI_TransmitReceive(&hspi1, (uint8_t*)txData, rxData, frameSize, 1000);
#else
        res = HAL_SPI_TransmitReceive_DMA(&hspi1, (uint8_t*)txData, rxData, frameSize);
#endif
        g_rxtxRunning = 2;
    }
    else {
#if !ASYNC_SPI
        res = HAL_SPI_Transmit(&hspi1, (uint8_t*)txData, frameSize, 1000);
#else
        res = HAL_SPI_Transmit_DMA(&hspi1, (uint8_t*)txData, frameSize);
#endif
        g_rxtxRunning = 1;
    }

    if (res == HAL_OK) {
        // print( "Sending %d bytes ... \n", frameSize );
        retval = STATUS_OK;
    }
    else if (res == HAL_BUSY) {
        // print("Hal device is busy. \n" );
        retval = STATUS_BUSY;
        HAL_GPIO_WritePin(s->chipSelectPort, s->chipSelectPin, !s->csActiveVal);
    }
    else {
        // print( "Transmit error. \n" );
        retval = ERROR_ABORTED;
        HAL_GPIO_WritePin(s->chipSelectPort, s->chipSelectPin, !s->csActiveVal);
    }

#if !ASYNC_SPI
    // print("Callback call after blocking transfer\n");
    HAL_GPIO_WritePin(s->chipSelectPort, s->chipSelectPin, !s->csActiveVal);
    s->latestTransferStatus = retval;
    if (callback)
        QueueTimer(timer_cb__spi, s, 15);
    g_rxtxRunning = 0;
#endif

    irq_unlock();
    return retval;
}

status_t S2PI_Abort(void)
{
    // Abort is inavailable.
    return STATUS_OK;
}

static s2pi_irq_callback_t g_cbIrq;
static void*               g_cbIrqObj;

void S2PI_IRQ_Callback()
{
    if (g_cbIrq) {
        print("IRQ function call. \n");
        g_cbIrq(g_cbIrqObj);
    }
}

status_t S2PI_SetIrqCallback(s2pi_slave_t        slave,
                             s2pi_irq_callback_t callback,
                             void*               callbackData)
{
    g_cbIrq    = callback;
    g_cbIrqObj = callbackData;
    return STATUS_OK;
}

status_t S2PI_CycleCsPin(s2pi_slave_t slave)
{
    struct slave_desc* s = g_slaves + slave;
    HAL_GPIO_WritePin(s->chipSelectPort, s->chipSelectPin, s->csActiveVal);
    HAL_GPIO_WritePin(s->chipSelectPort, s->chipSelectPin, !s->csActiveVal);
    return STATUS_OK;
}
