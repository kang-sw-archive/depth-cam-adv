/// @brief
///
/// @file program.h
/// @author Seungwoo Kang (ki6080@gmail.com)
/// @date 2019-12-27
/// @copyright Copyright (c) 2019. Seungwoo Kang. All rights reserved.
///
/// @details
#pragma once
#include "motor.h"
#include "time.h"
#include "uEmbedded/event-procedure.h"
#include "uEmbedded/queue_buffer.h"
#include "uEmbedded/timer_logic.h"
#include "uEmbedded/transceiver.h"
#include <main.h>
#include <platform/argus_log.h>

//=====================================================================//
//
// DEFINES
//
//=====================================================================//
// -- Default parameters
#define EVENT_QUEUE_BUFFER_SIZE        4096
#define RECEIVE_QUEUE_BUFER_SIZE       1024
#define TIMER_BUFFER_SIZE              2048
#define LOGGING_TEXT_QUEUE_BUFFER_SIZE 2048
#define EVENT_TIMER_QUEUE_BUFFER_SIZE  2048
#define TRANSMIT_QUEUE_BUFFER_SIZE     8096

// -- Capture-related parameters
// Number of pixels to be stored in the buffer before flush to host.
// Allocated byte size will be the value multiplied by 6 from given constant.
#define CAPTURED_LINE_BUFFER_SIZE 128

//=====================================================================//
//
// Interface
//
//=====================================================================//
/// A queue that holds data incoming. Should only be used in communication modules (Ethernet, USB, ...)
extern queue_buffer_t g_recvQueue;

/// Transceiver reference for adaptive operation
transceiver_vptr_t g_trans;

/// Event queue that helps executing asynchronous events safely.
extern struct EventQueue g_event;


/// Callbacks for each EXTI lines.
extern void ( *EXTI_CALLBACK_LINE[16] )( void );

/// Put log string to host.
void logprint( char const* str, ... );

/// Put string. No overhead.
void logputs( const char* str );

//! @brief      Global motor drive clock
extern int gMotorDriveClk;

/// Motors
extern motor_t g_motorX;
extern motor_t g_motorY;

//! Set motor step clock speed.
//! @param clk
void Motor_SetClkAll( int clk );

/// Split string by space character.
/// @param begin        Modifiable string character
/// @param o_argv       String pointer array contains output values of function
/// @param o_argc       Number of argv
/// @param capacity     Capacity of argv array.
void SplitStringBySpace( char* begin, char** o_argv, int* o_argc, size_t capacity );

/// Queue event timer.
timer_handle_t QueueEventTimer( void ( *eventCallback )( void* ), void* callbackObj, uint32_t microSec );

/// Transmit data to host.
/// @param data
/// @param datlen
/// @return Returns datlen always.
int TransmitToHost( void* data, size_t datlen );

/// Queue Event Wrapper
void AppQueueEvent( void ( *eventCallback )( void* ), void* cbData, size_t dataSz );

/// Abort event timer
void AbortEventTimer( timer_handle_t );