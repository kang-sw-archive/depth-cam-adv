#include <main.h>
#include "../s2pi.h"

/* <>---------------------------------------------------------------->
   *
   * GPIO FUNCTIONALITIES
   *
   <----------------------------------------------------------------<> */
///*!	The enumeration of S2PI pins. */
//    typedef enum S2PI_Pins
//    {
//        /*! The SPI clock pin. */
//        S2PI_CLK,
//
//        /*! The SPI chip select pin. */
//        S2PI_CS,
//
//        /*! The SPI MOSI pin. */
//        S2PI_MOSI,
//
//        /*! The SPI MISO pin. */
//        S2PI_MISO,
//
//        /*! The IRQ pin. */
//        S2PI_IRQ
//
//    } s2pi_pin_t;

//=====================================================================//
//
// <+> Statics
//
//=====================================================================//
struct pinstruct
{
    GPIO_TypeDef* port;
    uint16_t      pin;
    bool          bWrite;
};

typedef struct pinstruct pinstruct_t;
extern __IO bool         g_isGpioMode;

static pinstruct_t pins[5] = {
#ifdef STM32G431xx
  // CLK
  { .port = GPIOA, .pin = GPIO_PIN_5, .bWrite = false },
  // CS
  { .port = ARGUS_CS_GPIO_Port, .pin = ARGUS_CS_Pin, .bWrite = false },
  // MOSI
  { .port = GPIOA, .pin = GPIO_PIN_7, .bWrite = false },
  // MISO
  { .port = GPIOA, .pin = GPIO_PIN_6, .bWrite = false },
  // IRQ
  { .port = ARGUS_IRQ_GPIO_Port, .pin = ARGUS_IRQ_Pin, .bWrite = false },
#else
  // CLK
  { .port = GPIOA, .pin = GPIO_PIN_5, .bWrite = false },
  // CS
  { .port = GPIOC, .pin = GPIO_PIN_0, .bWrite = false },
  // MOSI
  { .port = GPIOA, .pin = GPIO_PIN_7, .bWrite = false },
  // MISO
  { .port = GPIOA, .pin = GPIO_PIN_6, .bWrite = false },
  // IRQ
  { .port = GPIOC, .pin = GPIO_PIN_1, .bWrite = false },
#endif
};

#define countof( v ) ( sizeof( v ) / sizeof( *v ) )

status_t S2PI_CaptureGpioControl( void )
{
    g_isGpioMode = true;
    size_t i;

    for ( i = 0; i < countof( pins ); i++ )
    {
        pins[i].bWrite = false;

        GPIO_InitTypeDef init;
        init.Mode  = GPIO_MODE_INPUT;
        init.Pin   = pins[i].pin;
        init.Pull  = GPIO_PULLUP;
        init.Speed = GPIO_SPEED_FREQ_MEDIUM;
        HAL_GPIO_Init( pins[i].port, &init );
    }
    return STATUS_OK;
}

status_t S2PI_ReleaseGpioControl( void )
{
    GPIO_InitTypeDef GPIO_InitStruct;

    // Configure SPI gpio pins
    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
    */
    GPIO_InitStruct.Pin       = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init( GPIOA, &GPIO_InitStruct );

    /*Configure GPIO pins : PC0 */
    GPIO_InitStruct.Pin   = ARGUS_CS_Pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init( ARGUS_CS_GPIO_Port, &GPIO_InitStruct );

    /*Configure GPIO pin : PC1 */
    GPIO_InitStruct.Pin  = ARGUS_IRQ_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init( ARGUS_IRQ_GPIO_Port, &GPIO_InitStruct );

    HAL_GPIO_WritePin( ARGUS_IRQ_GPIO_Port, GPIO_PIN_1, GPIO_PIN_SET );
    g_isGpioMode = false;
    return STATUS_OK;
}

status_t
S2PI_WriteGpioPin( s2pi_slave_t nouse_, s2pi_pin_t pin, uint32_t value )
{
    pinstruct_t* p = pins + pin;

    if ( p->bWrite == false )
    {
        GPIO_InitTypeDef init;
        init.Mode  = GPIO_MODE_OUTPUT_PP;
        init.Pin   = p->pin;
        init.Speed = GPIO_SPEED_MEDIUM;
        init.Pull  = GPIO_PULLUP;
        HAL_GPIO_Init( p->port, &init );

        p->bWrite = true;
    }

    HAL_GPIO_WritePin( p->port, p->pin, value );
    return STATUS_OK;
}

status_t
S2PI_ReadGpioPin( s2pi_slave_t nouse_, s2pi_pin_t pin, uint32_t* value )
{
    pinstruct_t* p = pins + pin;

    if ( p->bWrite == true )
    {
        GPIO_InitTypeDef init;
        init.Mode  = GPIO_MODE_INPUT;
        init.Pin   = p->pin;
        init.Pull  = GPIO_NOPULL;
        init.Speed = GPIO_SPEED_FREQ_MEDIUM;
        HAL_GPIO_Init( p->port, &init );

        p->bWrite = false;
    }

    *value = HAL_GPIO_ReadPin( p->port, p->pin );
    return STATUS_OK;
}
