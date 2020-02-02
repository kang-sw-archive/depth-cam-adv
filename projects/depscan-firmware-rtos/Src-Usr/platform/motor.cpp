//! @file       motor.c
//! @brief      Stepper motor implementation
//! @ingroup    Depscan_HW_Motor
//!
//! @author     Seungwoo Kang (ki6080@gmail.com)
//! @copyright  Copyright (c) 2019. Seungwoo Kang. All rights reserved.
//!
//! @details     This motor interface is the interface for driving the stepping
//!             motor. Suppose you have a one-way system where there is no
//!             positional feedback means like a decoder and the steps output
//!             without error.
//!              The motor's step signal is generated via a timer PWM, and
//!             smooth speed adjustment is possible by adjusting the PWM cycle
//!             at the interrupt called every motor clock cycle.
//!              However, to reduce the overhead as much as possible in fast
//!             motor cycles, the ISR needs to be as simple as possible.
#include <FreeRTOS.h>

#include <algorithm>
#include <main.h>
#include <stm32f4xx_hal.h>
#include "../app/app.h"
#include "../app/motor.h"

/////////////////////////////////////////////////////////////////////////////
// Statics
//
struct motor__
{
    int        ticks_left = 0;
    int        position   = 0;
    motor_cb_t cb;
    void*      cb_obj;

    // Calculates motor speed / accelerations
    usec_t phy_prev_tick = 0;
    float  phy_accel     = 10000000.f;
    float  phy_speed     = 0.f;
    float  phy_maxs      = 10000.f;
    float  phy_mins      = 200.f;

    // for hw layer use ...
    int hwid_;
    void ( *DIR )( motor_t, bool is_fwd );
    void ( *START )( motor_t );
};

static motor__ mx;
static motor__ my;

/////////////////////////////////////////////////////////////////////////////
// Exported variables
//
motor_t gMotX = &mx;
motor_t gMotY = &my;

/////////////////////////////////////////////////////////////////////////////
// Decls
//
//! @brief      Update motor on every motor ticks
//! @returns    Desired clock speed in Hz. 0 to stop.
static int update_motor( motor_t );

//! @brief      Start pwm generation.
static void trig_pwm( motor_t );

/////////////////////////////////////////////////////////////////////////////
// Defs
//
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;

#define MX_HTIM htim1
#define MY_HTIM htim3

#define MX_TIM_CLK SystemCoreClock
#define MY_TIM_CLK SystemCoreClock

/////////////////////////////////////////////////////////////////////////////
// Hardware dependent codes
//
extern "C" void InitMotors()
{
}

// IRQs
static void irq__( TIM_HandleTypeDef* htim, int clk )
{
    __HAL_TIM_CLEAR_IT( htim, TIM_IT_UPDATE );

    auto next_frq = update_motor( gMotX );
    if ( next_frq <= 0 ) {
        __HAL_TIM_DISABLE( htim );
        __HAL_TIM_SET_COUNTER( htim, 0 );
        return;
    }

    // Calculate auto-reload value from frequency
    __HAL_TIM_SET_AUTORELOAD( htim, clk / next_frq - 1 );
}

extern "C" void TIM1_UP_TIM10_IRQHandler()
{
    irq__( &MX_HTIM, MX_TIM_CLK );
}

extern "C" void TIM3_IRQHandler()
{
    irq__( &MY_HTIM, MY_TIM_CLK );
}

/////////////////////////////////////////////////////////////////////////////
// Interface impls
//
extern "C" {
//! @brief      Get motor status
motor_status_t Motor_Stat( motor_t m )
{
    return m->ticks_left ? MOTOR_STATE_MOVING : MOTOR_STATE_IDLE;
}

//! @brief      Set motor's desired max speed in Hz
motor_status_t Motor_SetMaxSpeed( motor_t m, uint32_t value )
{
    m->phy_maxs = std::max( m->phy_mins, (float)value );
    return MOTOR_OK;
}

//! @brief      Get motor's desired max speed in Hz
uint32_t Motor_GetMaxSpeed( motor_t m )
{
    return (uint32_t)m->phy_maxs;
}

motor_status_t Motor_SetMinSpeed( motor_t m, uint32_t value )
{
    m->phy_mins = std::min( m->phy_maxs, (float)value );
    return MOTOR_OK;
}

uint32_t Motor_GetMinSpeed( motor_t m )
{
    return uint32_t( m->phy_mins );
}

//! @brief      Set motor's acceleration [Hz/s]
motor_status_t Motor_SetAcceleration( motor_t, uint32_t value )
{
}

//! @brief      Set motor's acceleration [Hz/s]
uint32_t Motor_GetAcceleration( motor_t )
{
}

motor_status_t Motor_MoveBy( motor_t, int steps, motor_cb_t* cb, void* obj )
{
    return motor_status_t();
}

motor_status_t Motor_MoveTo( motor_t, int steps, motor_cb_t* cb, void* obj )
{
    return motor_status_t();
}
}

//! @brief      Get motor's position
//! @returns    motor's current position from origin point. Units are steps.
int Motor_Pos( motor_t )
{
}

//! @brief      Get motor's velocity.
//! @returns    velocity. \n
//!              Since the output value can be negative value, it'll return
//!             zero if the motor is in error state. To get exact error status
//!             of motor, @ref Motor_Stat function must be used.
int Motor_Velocity( motor_t )
{
}

//! @brief      Reset motor's origin to zero.
motor_status_t Motor_ResetPos( motor_t )
{
}

//! @brief       Stop motor immediately. This does not assure exact motor
//!             position
motor_status_t Motor_EmergencyStop( motor_t m )
{
}

int update_motor( motor_t m )
{
    return m->ticks_left != 0;
}
