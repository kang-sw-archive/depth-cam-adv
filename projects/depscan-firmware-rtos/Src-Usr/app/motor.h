#pragma once
#include <stdbool.h>
#include <stdint.h>

#if defined( __cplusplus )
extern "C" {
#endif // defined(__cplusplus)

//! @addtogroup Depscan
//! @{
//! @addtogroup Depscan_HW
//! @{
//! @defgroup   Depscan_HW_Motor
//! @brief      This motor class only provides an interface for handling
//!             stepper motors.
//! @details
//!              Since motors are very hardware dependent, all motor
//!             initialization must already be done on the platform stage before
//!             use. Therefore, the motor handle must already be defined and
//!             available in the hardware association layer header, such as
//!             hal.h, rather than created through a function call.
//! @{

//! @brief      Motor handle type
typedef struct motor__* motor_hnd_t;

//! @brief      Motor callback type
typedef void ( *motor_cb_t )( motor_hnd_t, void* );

//! @brief      An constant label that indicates motor status
typedef enum
{
    MOTOR_STATE_IDLE   = 0,
    MOTOR_STATE_MOVING = 1,
    MOTOR_STATE_ABORT  = -0x8001,

    MOTOR_OK    = 0,
    MOTOR_BUSY  = 2,
    MOTOR_ABORT = -1,
} motor_status_t;

//! @brief      Get motor status
motor_status_t Motor_Stat( motor_hnd_t );

//! @brief      Set motor's desired max speed in Hz
motor_status_t Motor_SetMaxSpeed( motor_hnd_t, uint32_t value );

//! @brief      Get motor's desired max speed in Hz
uint32_t Motor_GetMaxSpeed( motor_hnd_t );

//! @brief      Set motor's desired min speed in Hz
motor_status_t Motor_SetMinSpeed( motor_hnd_t, uint32_t value );

//! @brief      Get motor's desired min speed in Hz
uint32_t Motor_GetMinSpeed( motor_hnd_t );

//! @brief      Set motor's acceleration [Hz/s]
motor_status_t Motor_SetAcceleration( motor_hnd_t, uint32_t value );

//! @brief      Set motor's acceleration [Hz/s]
uint32_t Motor_GetAcceleration( motor_hnd_t );

//! @brief      Request relative movement
//! @returns    MOTOR_OK if successfully requested movement.
motor_status_t Motor_MoveBy( motor_hnd_t, int steps, motor_cb_t cb, void* obj );

//! @brief      Request absolute movement
//! @returns    MOTOR_OK if successfully requested movement.
motor_status_t
Motor_MoveTo( motor_hnd_t, int destination, motor_cb_t cb, void* obj ); 

//! @brief      Get motor's acceleration range
uint16_t Motor_GetAccelRange( motor_hnd_t );

//! @brief      Get motor's position
//! @returns    motor's current position from origin point. Units are steps.
int Motor_Pos( motor_hnd_t );

//! @brief      Get motor's velocity.
//! @returns    velocity. \n
//!              Since the output value can be negative value, it'll return
//!             zero if the motor is in error state. To get exact error status
//!             of motor, @ref Motor_Stat function must be used.
int Motor_Velocity( motor_hnd_t );

//! @brief      Reset motor's origin to zero.
motor_status_t Motor_ResetPos( motor_hnd_t );

//! @brief       Stop motor immediately. This does not assure exact motor
//!             position preserve
motor_status_t Motor_EmergencyStop( motor_hnd_t m );

//! @}
//! @}
//! @}

#if defined( __cplusplus )
}
#endif // defined(__cplusplus)