/** ------------------------------- **
   @brief Architecture independent logics for motor
   
   @file hw-motor.c
   @author Seungwoo Kang (ki6080@gmail.com)
   @version 0.1
   @date 2019-12-24
   @copyright Copyright (c) 2019. Seungwoo Kang. All rights reserved.
   
   @details
 **/
#include "motor.h"
#include "arch/irq.h"
#include "program.h"
#include <stm32f4xx_hal.h>
#include <string.h>

static void
dir_null( motor_t* v, motor_fwd_t d )
{
}
static void actv_null( motor_t* v, bool b ) {}

static struct motor_vtable mvtbl_null = { dir_null, actv_null };
//! @todo. Verify motor interface
void InitMotors()
{
    memset( &g_motorY, 0, sizeof( g_motorY ) );
    memset( &g_motorX, 0, sizeof( g_motorX ) );

    g_motorX.vtable = &mvtbl_null;
    g_motorY.vtable = &mvtbl_null;

    g_motorX.OnMovementDone = NULL;
    g_motorY.OnMovementDone = NULL;

    HW_INIT_MOTORS( &g_motorX, &g_motorY );
}

void Motor_Stop_Emergency( motor_t* m )
{
    m->vtable->ACTIVATE( m, false );
    m->Status = MOTOR_IDLE;
    AbortTimer( m->hTimer );
}

static void Motor_TimerCallback( void* vp_m )
{
    // logprint("Motor timer called --> Current : %u\n", timer_nextTrigger(&g_timer), SYSTEM_TIM_TICK());
    motor_t* m = (motor_t*)vp_m;

    // Stop motor
    m->vtable->ACTIVATE( m, false );

    // Call callback
    if ( m->OnMovementDone )
        m->OnMovementDone( m->OnMovementDoneObj );

    // Handle motor error
    if ( m->PendingTicks ) {
        int tolerance = ( m->interval_us >> 1 );
        int error_us  = LTime_Diff( GetLTime(), m->lastTrigger ) - m->TicksToDelay;
        if ( error_us > tolerance ) {
            int fix = error_us / m->interval_us + 1;
            // print( "Motor interval error: %d .. for interval %d ... fixing by %d steps\n", error_us, m->interval_us, fix );
            m->error += ( m->PendingTicks > 0 ) * ( fix << 1 ) - fix;
        }
    }

    // Refresh motor status
    m->Position += m->PendingTicks;
    m->PendingTicks = 0;
    m->Status       = MOTOR_IDLE;
}

int Motor_MoveBy( motor_t* m, int32_t ticks )
{
    // Motor operation can be permitted only when idle state
    if ( m->Status != MOTOR_IDLE )
        return m->Status;

    ticks -= m->error;
    m->error = 0;
    if ( ticks == 0 ) {
        Motor_TimerCallback( m );
        return MOTOR_SUCCESS;
    }

    m->lastTrigger = GetLTime();

    // Lock IRQ before queue timer.
    irq_lock();
    m->Status       = MOTOR_BUSY;
    m->PendingTicks = ticks;

    // Set motor direction
    m->vtable->DIR( m, ticks > 0 );

    // Queue timer before activate motor movement.
    size_t TicksToDelay = m->interval_us * ( ticks > 0 ? ticks : -ticks );
    m->hTimer           = QueueTimer( Motor_TimerCallback, m, TicksToDelay );
    m->TicksToDelay     = TicksToDelay;

    // logprint( "Motor timer queued --> Trig : %u, Current : %u\n", timer_nextTrigger(&g_timer), SYSTEM_TIM_TICK() );
    uassert( m->hTimer.n != NULL );

    // Activate motor.
    m->vtable->ACTIVATE( m, true );
    irq_unlock();

    return MOTOR_SUCCESS;
}

int Motor_MoveTo( motor_t* m, int32_t pos )
{
    return Motor_MoveBy( m, pos - m->Position );
}

bool Motor_SetClk( motor_t* m, int clk )
{
    if ( m->DriveClk != clk ) {
        int res = HW_MOT_SET_CLK( m, clk );
        if ( res < 0 ) {
            return false;
        }
        m->DriveClk    = clk;
        m->interval_us = res;
    }
    return true;
}
