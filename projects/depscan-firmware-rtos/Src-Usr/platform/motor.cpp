//! @file motor.c
//! @brief Stepper motor implementation
//!
//! @author Seungwoo Kang (ki6080@gmail.com)
//! @copyright Copyright (c) 2019. Seungwoo Kang. All rights reserved.
//!
//! @details
#include "../app/motor.h"

//! @todo Implement this motor implementation

//! @brief      Get motor status
motor_status_t Motor_Stat(motor_t);

//! @brief      Set motor's desired max speed in Hz
motor_status_t Motor_SetMaxSpeed(motor_t, uint32_t value);

//! @brief      Get motor's desired max speed in Hz
uint32_t Motor_GetMaxSpeed(motor_t);

//! @brief      Set motor's acceleration [Hz/s]
motor_status_t Motor_SetAcceleration(motor_t, uint32_t value);

//! @brief      Set motor's acceleration [Hz/s]
uint32_t Motor_GetAcceleration(motor_t);

//! @brief      Request relative movement
//! @returns    MOTOR_OK if successfully requested movement.
motor_status_t Motor_MoveBy(motor_t, int steps);

//! @brief      Request absolute movement
//! @returns    MOTOR_OK if successfully requested movement.
motor_status_t Motor_MoveTo(motor_t, int steps);

//! @brief      Get motor's position
//! @returns    motor's current position from origin point. Units are steps.
int Motor_Pos(motor_t);

//! @brief      Get motor's velocity.
//! @returns    velocity. \n
//!              Since the output value can be negative value, it'll return
//!             zero if the motor is in error state. To get exact error status
//!             of motor, @ref Motor_Stat function must be used.
int Motor_Velocity(motor_t);

//! @brief      Reset motor's origin to zero.
motor_status_t Motor_ResetPos(motor_t);

//! @brief       Stop motor immediately. This does not assure exact motor
//!             position
motor_status_t Motor_EmergencyStop(motor_t m);

