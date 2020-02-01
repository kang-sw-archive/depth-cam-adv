/*! \brief
    \file s2pi-1.c
    \author Seungwoo Kang (ki6080@gmail.com)
    \version 0.1
    \date 2019-09-14

    \copyright Copyright (c) 2019. Seungwoo Kang. All rights reserved.

 */
#include <FreeRTOS.h>

#include <platform/argus_s2pi.h>
#include "../s2pi.h"

#include <uEmbedded/uassert.h>

#include <platform/argus_irq.h>
#include <stdbool.h>
#include <stdlib.h>
#include <uEmbedded/event-procedure.h>
#include "stm32f4xx_hal.h"

#include "../../app/app.h"

/////////////////////////////////////////////////////////////////////////////
// Definitions

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

#define ASYNC_SPI    0
#define print( ... ) // for(volatile size_t i = 155; --i; )

// All slaves
__IO struct slave_desc g_slaves[S2PI_SLAVE_MAX] = {
    // ETHERNET SPI
    { .chipSelectPort       = NULL,
      .csActiveVal          = 0,
      .callback             = NULL,
      .callbackParam        = NULL,
      .chipSelectPin        = GPIO_PIN_2,
      .latestTransferStatus = STATUS_OK },
    // SENSOR SPI
    { .chipSelectPort       = GPIOC,
      .csActiveVal          = 0,
      .callback             = NULL,
      .callbackParam        = NULL,
      .chipSelectPin        = GPIO_PIN_0,
      .latestTransferStatus = STATUS_OK },
};

// Indicates the slave which occupying the spi device.
static __IO s2pi_slave_t g_activeSlave = S2PI_SLAVE_NONE;

// Indicates whether the rx / tx operation has done.
// Runs as semaphore.
__IO uint8_t g_rxtxRunning = 0;

// Checks if SPI device is currently in GPIO mode.
__IO bool g_isGpioMode = false;

extern SPI_HandleTypeDef hspi1;

extern DMA_HandleTypeDef hdma_spi1_tx;
extern DMA_HandleTypeDef hdma_spi1_rx;

/* <>---------------------------------------------------------------->
   *
   * TRANSFER FUNCTIONS
   *
   <----------------------------------------------------------------<> */

//
//void HAL_SPI_AbortCpltCallback( SPI_HandleTypeDef* hspi )
//{
//    transfer_done( ERROR_ABORTED );
//}
static void timer_cb__spi( void* p )
{
    struct slave_desc* s = (struct slave_desc*)p;
    s->callback( s->latestTransferStatus, s->callbackParam );
}

#if ASYNC_SPI
static void transfer_done( DMA_HandleTypeDef* h )
{
    if ( g_rxtxRunning > 0 ) {
        g_rxtxRunning--;
        return;
    }

    struct slave_desc volatile* s = g_slaves + g_activeSlave;
    HAL_GPIO_WritePin( s->chipSelectPort, s->chipSelectPin, !s->csActiveVal );

    // print("Receiving Bytes ...\n");
    if ( s->callback ) {
        // logputs("Calling callback\n");
        // s->callback( val, s->callbackParam );
        s->latestTransferStatus = STATUS_OK;
        API_SetTimerFromISR( 1, (void*)s, timer_cb__spi );
    }
}

static void transfer_error( DMA_HandleTypeDef* h )
{
    if ( g_rxtxRunning > 0 ) {
        g_rxtxRunning--;
        return;
    }

    struct slave_desc volatile* s = g_slaves + g_activeSlave;
    HAL_GPIO_WritePin( s->chipSelectPort, s->chipSelectPin, !s->csActiveVal );
    if ( s->callback ) {
        s->latestTransferStatus = ERROR_FAIL;
        API_SetTimerFromISR( 1, (void*)s, timer_cb__spi );
    }
}
 
void HAL_SPI_ErrorCallback( SPI_HandleTypeDef* hspi )
{
    transfer_error( NULL );
}

void HAL_SPI_TxCpltCallback( SPI_HandleTypeDef* nouse_ )
{
    transfer_done( NULL );
}

void HAL_SPI_RxCpltCallback( SPI_HandleTypeDef* nouse_ )
{
    transfer_done( NULL );
}

void HAL_SPI_TxRxCpltCallback( SPI_HandleTypeDef* nouse_ )
{
    transfer_done( NULL );
}
#endif

void S2PI_Init()
{
    size_t i;

    for ( i = 0; i < S2PI_SLAVE_MAX; i++ ) {
        struct slave_desc volatile* s = g_slaves + i;
        HAL_GPIO_WritePin( s->chipSelectPort, s->chipSelectPin, !s->csActiveVal );
    }
}

void S2PI_TransferFrameSync( s2pi_slave_t slave, uint8_t const* txData, uint8_t* rxData, size_t frameSize, s2pi_callback_t callback, void* callbackData )
{
    while ( S2PI_TransferFrame( slave, txData, rxData, frameSize, callback, callbackData ) != STATUS_OK ) { }
    while ( S2PI_GetStatus() != STATUS_IDLE ) { }
}

// Some random number.
uint32_t S2PI_GetBaudRate( void )
{
    return (uint32_t)12e6;
}

status_t S2PI_SetBaudRate( uint32_t baudRate_Bps )
{
    // No effect
    return STATUS_OK;
}

status_t S2PI_GetStatus( void )
{
    if ( g_rxtxRunning ) {
        return STATUS_BUSY;
    }
    else if ( g_isGpioMode ) {
        return STATUS_S2PI_GPIO_MODE;
    }
    else {
        return STATUS_IDLE;
    }
}

status_t S2PI_TransferFrame( s2pi_slave_t    slave,
                             uint8_t const*  txData,
                             uint8_t*        rxData,
                             size_t          frameSize,
                             s2pi_callback_t callback,
                             void*           callbackData )
{
    uassert( txData );
    uassert( frameSize );

    // print("T:%db \n", frameSize);

    status_t stat = S2PI_GetStatus();

    if ( slave >= S2PI_SLAVE_MAX )
        return ERROR_S2PI_INVALID_SLAVE;
    if ( stat != STATUS_IDLE )
        return stat;

    IRQ_LOCK();
    g_activeSlave        = slave;
    struct slave_desc* s = (struct slave_desc*)&g_slaves[g_activeSlave];
    s->callback          = callback;
    s->callbackParam     = callbackData;

    // Select slave. Deselect happens when transmission done.
    HAL_GPIO_WritePin( s->chipSelectPort, s->chipSelectPin, s->csActiveVal );

    HAL_StatusTypeDef res;
    status_t          retval;

    if ( rxData ) {
        g_rxtxRunning = 1;
#if !ASYNC_SPI
        res = HAL_SPI_TransmitReceive( &hspi1, (uint8_t*)txData, rxData, frameSize, 1000 );
#else
        res = HAL_SPI_TransmitReceive_DMA( &hspi1, (uint8_t*)txData, rxData, frameSize );
#endif
    }
    else {
        g_rxtxRunning = 1;
#if !ASYNC_SPI
        res = HAL_SPI_Transmit( &hspi1, (uint8_t*)txData, frameSize, 1000 );
#else
        res = HAL_SPI_Transmit_DMA( &hspi1, (uint8_t*)txData, frameSize );
#endif
    }

    if ( res == HAL_OK ) {
        // print( "Sending %d bytes ... \n", frameSize );
        retval = STATUS_OK;
    }
    else if ( res == HAL_BUSY ) {
        // print("Hal device is busy. \n" );
        retval = STATUS_BUSY;
        HAL_GPIO_WritePin( s->chipSelectPort, s->chipSelectPin, !s->csActiveVal );
    }
    else {
        // print( "Transmit error. \n" );
        retval = ERROR_ABORTED;
        HAL_GPIO_WritePin( s->chipSelectPort, s->chipSelectPin, !s->csActiveVal );
    }

#if !ASYNC_SPI
    // print("Callback call after blocking transfer\n");
    HAL_GPIO_WritePin( s->chipSelectPort, s->chipSelectPin, !s->csActiveVal );
    s->latestTransferStatus = retval;
    if ( callback )
        API_SetTimer( 15, s, timer_cb__spi );
    g_rxtxRunning = 0;
#endif

    IRQ_UNLOCK();
    return retval;
}

status_t S2PI_Abort( void )
{
    // Abort is inavailable.
    return STATUS_OK;
}

status_t S2PI_CycleCsPin( s2pi_slave_t slave )
{
    struct slave_desc volatile* s = g_slaves + slave;
    HAL_GPIO_WritePin( s->chipSelectPort, s->chipSelectPin, s->csActiveVal );
    HAL_GPIO_WritePin( s->chipSelectPort, s->chipSelectPin, !s->csActiveVal );
    return STATUS_OK;
}
