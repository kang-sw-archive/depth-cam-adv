/*! \brief
    \file hw-motor.h
    \author Seungwoo Kang (ki6080@gmail.com)
    \version 0.1
    \date
    
    \copyright Copyright (c) 2019. Seungwoo Kang. All rights reserved.
    \details */
#pragma once
#include "arch/hw-timer.h" 
#include "time.h"
#include <stdbool.h>
#include <stdint.h>

//! \brief      Motor direction type. false if backward.
typedef bool motor_fwd_t;

//! \brief      Motor status notify
typedef enum { MOTOR_INVALID_STATE = -1,
               MOTOR_IDLE          = 0,
               MOTOR_BUSY          = 1 } motor_status_t;

//! \brief      Motor API error code
enum {
    MOTOR_SUCCESS          = 0,
    ERR_MOTOR_FAILED       = -1,
    ERR_MOTOR_INVALID_ARG  = -2,
    ERR_MOTOR_TIMER_FAILED = -3
};

/*! \brief      Motor descriptor */
typedef struct
{
    //! \brief  Custom value to identify hardware timer / driver.
    int HWID;

    //! \brief  Current motor position. Modifiable.
    int32_t Position;

    //! \brief  Pending movement ticks
    int32_t PendingTicks;

    //! \brief  Drive clk
    int32_t DriveClk;

    //! \Motor  drive interval us
    int32_t interval_us;

    //! \brief  Current motor status
    motor_status_t Status;

    //! \brief  Trigger time
    ltimetick_t lastTrigger;

    //! \brief  Original delay
    size_t TicksToDelay;

    //! \brief  Accumulated errors
    int32_t error;

    //! \brief  Callback table
    struct motor_vtable* vtable;

    //! \brief  Timer handle for counting clocks
    timer_handle_t hTimer;

    //! \brief  Callback function that'll be called when movement done
    void ( *OnMovementDone )( void* );

    //! \brief  Callback Object
    void* OnMovementDoneObj;
} motor_t;

/*! \brief      Motor hardware control callbacks */
struct motor_vtable {
    //! \brief  Set motor direction output
    void ( *DIR )( motor_t*, motor_fwd_t dir );

    //! \brief  Toggle motor enable / disable signal
    void ( *ACTIVATE )( motor_t*, bool value );
};

/*! \brief      Initialize motors. Hardware specific code. */
void InitMotors();

/** ------------------------------- **
   @brief Initialize X and Y motor instance.
   @param xmot X motor reference
   @param ymot Y motor reference
 **/
void HW_INIT_MOTORS( motor_t* xmot, motor_t* ymot );

//! Set motor drive clock in hardware
//! @param clk
//! @returns Actual microseconds per tick.
int HW_MOT_SET_CLK( motor_t* m, int clk );

/*! \brief      Stop motor movement instantly. This simply forces stopping the motor. Should only be used in emergency circumstances */
void Motor_Stop_Emergency( motor_t* m );

/*! \brief          Move motor by offset 
    \param ticks    Ticks to move. 
    \returns        Error code. */
int Motor_MoveBy( motor_t* m, int32_t ticks );

/*! \brief      Move motor by position 
    \param pos  Destination of motor movement
    \returns    Error code. */
int Motor_MoveTo( motor_t* m, int32_t pos );

//! @breif      Set motor drive clock speed
bool Motor_SetClk( motor_t* m, int clk );
