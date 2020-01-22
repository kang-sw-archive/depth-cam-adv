/** ------------------------------- **
   @brief Architecture dependent motor control logics
   
   @file hw-motor.c
   @author Seungwoo Kang (ki6080@gmail.com)
   @version 0.1
   @date 2019-12-24
   @copyright Copyright (c) 2019. Seungwoo Kang. All rights reserved.
   
   @details
    
 **/
#include "stm32f4xx_hal.h"

#include "app/motor.h"

#include <stdint.h>
#include <stdlib.h>

extern TIM_HandleTypeDef htim1;           // HW timer reference
extern TIM_HandleTypeDef htim3;           // HW timer reference
extern uint32_t          SystemCoreClock; // System clock value

TIM_HandleTypeDef* const HTIM[] = { &htim1, &htim3 };

#define MOT_STEP_CLK   gMotorDriveClk
#define BASE_TIMER_CLK 1000000
#define INDEX_X        0
#define INDEX_Y        1

static void HW_MOT_DIR( motor_t*, motor_fwd_t );
static void HW_MOT_ACTIVE( motor_t*, bool );

static struct motor_vtable g_vt = { .DIR = HW_MOT_DIR, .ACTIVATE = HW_MOT_ACTIVE };

int HW_MOT_SET_CLK( motor_t* m, int clk )
{
    const uint32_t timClk      = SystemCoreClock;
    const uint32_t timPrescale = timClk / ( BASE_TIMER_CLK );
    const uint32_t timReload   = BASE_TIMER_CLK / clk;
    const uint32_t timOCR      = timReload >> 1;

    volatile TIM_TypeDef*          tims[2] = { htim1.Instance, htim3.Instance };
    volatile TIM_Base_InitTypeDef* timi[2] = { &htim1.Init, &htim3.Init };

    volatile TIM_TypeDef* htim = tims[m->HWID];
    htim->PSC                  = timPrescale - 1;
    htim->ARR                  = timReload;
    htim->CCR1                 = timOCR;
    htim->RCR                  = 0; // No reload required.
    htim->EGR                  = TIM_EGR_UG;

    return timReload;
}

void HW_INIT_MOTORS( motor_t* xmot, motor_t* ymot )
{
    HW_MOT_SET_CLK( xmot, 1000 );
    HW_MOT_SET_CLK( xmot, 1000 );

    xmot->HWID   = INDEX_X;
    xmot->vtable = &g_vt;

    ymot->HWID   = INDEX_Y;
    ymot->vtable = &g_vt;
}

static void HW_MOT_DIR( motor_t* s, motor_fwd_t dir )
{
    // Simply output to GPIO port
    static GPIO_TypeDef* const Port       = GPIOB;
    static int const           Pins[2]    = { GPIO_PIN_8, GPIO_PIN_9 };
    static GPIO_PinState       Dirs[2][2] = { { GPIO_PIN_SET, GPIO_PIN_RESET }, { GPIO_PIN_RESET, GPIO_PIN_SET } };

    int const pin = Pins[s->HWID];
    HAL_GPIO_WritePin( Port, pin, Dirs[s->HWID][dir == true] ); // dir == false ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void HW_MOT_ACTIVE( motor_t* s, bool val )
{
    // Turn on/off output compare channel.
    static int const   Channels[2] = { TIM_CHANNEL_1, TIM_CHANNEL_1 };
    TIM_HandleTypeDef* tim         = HTIM[s->HWID];

    if ( val ) {
        tim->Instance->CNT = 0;
        HAL_TIM_Base_Start( tim );
        HAL_TIM_PWM_Start( tim, Channels[s->HWID] );
    }
    else {
        HAL_TIM_PWM_Stop( tim, Channels[s->HWID] );
        HAL_TIM_Base_Stop( tim );
    }
}