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
#include <array>
#include <main.h>
#include <stm32f4xx_hal.h>
#include <uEmbedded/uassert.h>
#include "../app/app.h"
#include "../app/motor.h"

/////////////////////////////////////////////////////////////////////////////
// Statics
//
struct motor__
{
    int        pending_movement = 0;
    int        position         = 0;
    motor_cb_t cb               = nullptr;
    void*      cb_obj           = nullptr;

    // Calculates motor speed / accelerations
    usec_t phy_prev_time = 0;
    float  phy_accel     = 100000.f;
    float  phy_velocity  = 0.f;
    float  phy_maxs      = 10000.f;
    float  phy_mins      = 200.f;

    // Calculated on start sequence.
    float phy_progress      = 0.f;
    float phy_progress_step = 0.f;

    // for hw layer use ...
    int hwid_;
    void ( *DIR )( motor_hnd_t, bool is_fwd );
    void ( *START )( motor_hnd_t ); // Since this callback is invoked very first
                                    // of movement progress, it should be
                                    // implemented that triggers timer IRQ as
                                    // soon as possible, with shortest ARR

    float velocity() const { return phy_velocity; }
    float speed() const
    {
        return phy_velocity > 0 ? phy_velocity : -phy_velocity;
    }
};

static motor__ mx;
static motor__ my;

/////////////////////////////////////////////////////////////////////////////
// Exported variables
//
motor_hnd_t gMotX = &mx;
motor_hnd_t gMotY = &my;

/////////////////////////////////////////////////////////////////////////////
// Decls
//
//! @brief      Update motor on every motor ticks
//! @returns    Desired clock speed in Hz. 0 to stop.
static int update_motor( motor_hnd_t );

//! @brief      Start pwm generation.
static void trig_pwm( motor_hnd_t );

/////////////////////////////////////////////////////////////////////////////
// Defs
//
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;

#define HTIM_X htim1
#define HTIM_Y htim3

template <class ty__, size_t n__>
constexpr size_t countof( ty__ ( & )[n__] )
{
    return n__;
}

static TIM_HandleTypeDef* const htim_xy[]   = { &HTIM_X, &HTIM_Y };
static int const                clk_xy[]    = { int( 1e6 ), int( 1e6 ) };
static uint32_t const           ccr_ch_xy[] = { TIM_CHANNEL_1, TIM_CHANNEL_1 };
static GPIO_TypeDef* const      dir_gpio_xy[]
  = { MOT_DIR_1_GPIO_Port, MOT_DIR_2_GPIO_Port };
static uint16_t const      dir_gpio_pin_xy[] = { MOT_DIR_1_Pin, MOT_DIR_2_Pin };
static GPIO_PinState const dir_gpio_value_xy[][2] = {
  // [hwid][fwd?]
  { GPIO_PIN_RESET, GPIO_PIN_SET }, // Motor x
  { GPIO_PIN_RESET, GPIO_PIN_SET }, // Motor y
};

/////////////////////////////////////////////////////////////////////////////
// Hardware dependent codes
//
static void DIR_FNC( motor_hnd_t, bool is_fwd );
static void START_FNC( motor_hnd_t );

extern "C" void InitMotors()
{
    //! @todo       Implement this!
    auto motors = { &mx, &my };
    for ( size_t i = 0; i < motors.size(); i++ ) {
        auto m   = motors.begin()[i];
        m->hwid_ = i;
        m->START = START_FNC;
        m->DIR   = DIR_FNC;
    }

    // Timer clocks should be equal with TIM_CLK_X, TIM_CLK_Y
    // Enable update interrupt
    // Set CC mode as PWM
    for ( size_t i = 0; i < countof( htim_xy ); i++ ) {
        auto tim = htim_xy[i];
        auto prs = SystemCoreClock / clk_xy[i];
        auto ch  = ccr_ch_xy[i];
        auto m   = motors.begin()[i];

        __HAL_TIM_SET_PRESCALER( tim, prs );
        __HAL_TIM_ENABLE_IT( tim, prs );
        Motor_SetMaxSpeed( m, (uint32_t)m->phy_maxs ); // Since motor isn't
                                                       // driven by PWM itself
                                                       // but by the clock
                                                       // signal generated on
                                                       // the match, CCR can be
                                                       // a fixed value during
                                                       // operation. This
                                                       // implementation, simply
                                                       // set it as double of
                                                       // the maximum frequency.
    }
}

static void DIR_FNC( motor_hnd_t m, bool is_fwd )
{
    auto hwid = m->hwid_;
    uassert( abs( hwid ) <= 1 );

    HAL_GPIO_WritePin(
      dir_gpio_xy[hwid],
      dir_gpio_pin_xy[hwid],
      dir_gpio_value_xy[hwid][is_fwd] );
}

static void START_FNC( motor_hnd_t m )
{
    auto hwid = m->hwid_;
    uassert( abs( hwid ) <= 1 );

    auto tim = htim_xy[hwid];
    auto ch  = ccr_ch_xy[hwid];

    __HAL_TIM_SetCounter( tim, 0 );
    __HAL_TIM_CLEAR_IT( tim, TIM_IT_UPDATE );
    HAL_TIM_OC_Start( tim, ch );
}

// This function placed this section since it modifies CCR value, which means
// hardware associated.
motor_status_t Motor_SetMaxSpeed( motor_hnd_t m, uint32_t value )
{
    m->phy_maxs      = std::max( m->phy_mins, (float)value );
    auto period_us   = int( 1e6f / m->phy_maxs );
    auto compare_val = period_us / 2;

    auto tim = htim_xy[m->hwid_];
    auto ch  = ccr_ch_xy[m->hwid_];
    __HAL_TIM_SET_COMPARE( tim, ch, compare_val );

    return MOTOR_OK;
}

// IRQs
static void irq__( TIM_HandleTypeDef* htim, int ch, int clk )
{
    __HAL_TIM_CLEAR_IT( htim, TIM_IT_UPDATE );

    auto next_frq = update_motor( gMotX );
    if ( next_frq <= 0 ) {
        HAL_TIM_OC_Stop( htim, ch );
        return;
    }

    // Calculate auto-reload value from frequency
    // ActualPeriod := ARR * 1/TimClk [sec]
    // DesiredPeriod = 1/next_frq = ActualPeriod
    // 1/next_frq = ARR*1/TimClk
    // therefore ARR = TimClk/next_frq
    __HAL_TIM_SET_AUTORELOAD( htim, clk / next_frq - 1 );
}

extern "C" void TIM1_UP_TIM10_IRQHandler()
{
    irq__( &HTIM_X, ccr_ch_xy[0], clk_xy[0] );
}

extern "C" void TIM3_IRQHandler()
{
    irq__( &HTIM_Y, ccr_ch_xy[1], clk_xy[1] );
}

/////////////////////////////////////////////////////////////////////////////
// Interface impls
//
extern "C" {
//! @brief      Get motor status
motor_status_t Motor_Stat( motor_hnd_t m )
{
    return m->pending_movement != 0 ? MOTOR_STATE_MOVING : MOTOR_STATE_IDLE;
}

//! @brief      Get motor's desired max speed in Hz
uint32_t Motor_GetMaxSpeed( motor_hnd_t m )
{
    return (uint32_t)m->phy_maxs;
}

motor_status_t Motor_SetMinSpeed( motor_hnd_t m, uint32_t value )
{
    m->phy_mins = std::min( m->phy_maxs, (float)value );
    return MOTOR_OK;
}

uint32_t Motor_GetMinSpeed( motor_hnd_t m )
{
    return uint32_t( m->phy_mins );
}

//! @brief      Set motor's acceleration [Hz/s]
motor_status_t Motor_SetAcceleration( motor_hnd_t m, uint32_t value )
{
    m->phy_accel = (float)std::max( (uint32_t)100u, value );
}

//! @brief      Set motor's acceleration [Hz/s]
uint32_t Motor_GetAcceleration( motor_hnd_t m )
{
    return m->phy_accel;
}

motor_status_t
Motor_MoveBy( motor_hnd_t m, int steps, motor_cb_t cb, void* obj )
{
    // If motor's moving yet ...
    if ( m->pending_movement )
        return MOTOR_BUSY;

    // If it's 0 cycle request, simply execute callback and return.
    if ( steps == 0 ) {
        if ( cb )
            cb( m, obj );
        return MOTOR_OK;
    }

    // Setup movement properties
    m->cb               = cb;    // Specify callbacks
    m->cb_obj           = obj;   // & callback obj
    m->pending_movement = steps; // Use steps as itself.

    // Setup physical state calculation states
    m->phy_progress      = 0.f;                //
    m->phy_progress_step = 1.f / abs( steps ); // Normalized progress value
    m->phy_prev_time     = API_GetTime_us();   // To elapse delta seconds ...

    // Trigger movement
    m->DIR( m, steps > 0 );
    m->START( m );
}

motor_status_t
Motor_MoveTo( motor_hnd_t m, int destination, motor_cb_t cb, void* obj )
{
    return Motor_MoveBy( m, destination - m->position, cb, obj );
}

//! @brief      Get motor's position
//! @returns    motor's current position from origin point. Units are steps.
int Motor_Pos( motor_hnd_t m )
{
    return m->position;
}

//! @brief      Get motor's velocity.
//! @returns    velocity. \n
//!              Since the output value can be negative value, it'll return
//!             zero if the motor is in error state. To get exact error status
//!             of motor, @ref Motor_Stat function must be used.
int Motor_Velocity( motor_hnd_t m )
{
    return m->velocity();
}

//! @brief      Reset motor's origin to zero.
motor_status_t Motor_ResetPos( motor_hnd_t m )
{
    m->position = 0;
}

//! @brief       Stop motor immediately. This does not assure exact motor
//!             position
motor_status_t Motor_EmergencyStop( motor_hnd_t m )
{
    // Make it finish on next IRQ
    m->pending_movement
      = m->pending_movement * ( m->pending_movement > 0 ? 1 : -1 );
}
}

#define xor_( a, b ) ( !( a ) != !( b ) )

// Separate wrapper to call motor callback on timer session.
static void motor_tim_cb__( void* mt_ref )
{
    auto m = (motor_hnd_t)mt_ref;
    m->cb( m, m->cb_obj );
}

int update_motor( motor_hnd_t m )
{
    if ( abs( m->pending_movement ) <= 1 ) {
        if ( m->cb ) { // Make it invoke outside of ISR
            API_SetTimerFromISR( 0, m, motor_tim_cb__ );
            m->cb = nullptr; // Always reset callback
        }
        m->phy_velocity = 0.f;
        return 0;
    }
    auto  now   = API_GetTime_us();
    float delta = (float)( now - m->phy_prev_time ) * 1e-6f; // 1us

    // Determine direction and amount of acceleration
    bool const bFwd    = m->pending_movement > 0;
    bool const bRising = m->phy_progress < 0.5f;
    float      accel   = m->phy_accel * delta * (float)xor_( bFwd, bRising );

    m->phy_prev_time = now;
    m->phy_velocity += accel;
    m->phy_progress += m->phy_progress_step;
    m->pending_movement -= ( m->pending_movement > 0 ? 1 : -1 );

    // Clamp value in range
    return (int)std::max( m->phy_mins, std::min( m->phy_maxs, m->speed() ) );
}
