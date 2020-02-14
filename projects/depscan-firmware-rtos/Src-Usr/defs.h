#pragma once
#include <stdint.h>

//! @addtogroup Depscan
//! @{
//! @defgroup Depscan_Defs
//! @{
//! @defgroup Depscan_Defs_Constants
//! @{

#define USB_READ_BUF_SIZE 1024
// #define USB_WRITE_BUF_SIZE         1024
#define HOST_TRANSFER_BUFFER_SIZE     0x800
#define NUM_MAX_HWTIMER_NODE          20
#define NUM_TIMER_TASK_STACK_WORDS    768
#define NUM_MAX_EXPORT_BINARY         40
#define CAPTURER_BUFFER_SIZE          2048
#define CAPTURE_TASK_STACK_DEPTH      512
#define CAPTURE_NUM_MEASUREMENT_RETRY 5
#define CAPTURE_NUM_INITIAL_DISCARDS  4

enum TaskPriority
{
    TaskPriorityNone         = 0,      ///< No priority (not initialized).
    TaskPriorityIdle         = 1,      ///< Reserved for Idle thread.
    TaskPriorityLow          = 8,      ///< Priority: low
    TaskPriorityLow1         = 8 + 1,  ///< Priority: low + 1
    TaskPriorityLow2         = 8 + 2,  ///< Priority: low + 2
    TaskPriorityLow3         = 8 + 3,  ///< Priority: low + 3
    TaskPriorityLow4         = 8 + 4,  ///< Priority: low + 4
    TaskPriorityLow5         = 8 + 5,  ///< Priority: low + 5
    TaskPriorityLow6         = 8 + 6,  ///< Priority: low + 6
    TaskPriorityLow7         = 8 + 7,  ///< Priority: low + 7
    TaskPriorityBelowNormal  = 16,     ///< Priority: below normal
    TaskPriorityBelowNormal1 = 16 + 1, ///< Priority: below normal + 1
    TaskPriorityBelowNormal2 = 16 + 2, ///< Priority: below normal + 2
    TaskPriorityBelowNormal3 = 16 + 3, ///< Priority: below normal + 3
    TaskPriorityBelowNormal4 = 16 + 4, ///< Priority: below normal + 4
    TaskPriorityBelowNormal5 = 16 + 5, ///< Priority: below normal + 5
    TaskPriorityBelowNormal6 = 16 + 6, ///< Priority: below normal + 6
    TaskPriorityBelowNormal7 = 16 + 7, ///< Priority: below normal + 7
    TaskPriorityNormal       = 24,     ///< Priority: normal
    TaskPriorityNormal1      = 24 + 1, ///< Priority: normal + 1
    TaskPriorityNormal2      = 24 + 2, ///< Priority: normal + 2
    TaskPriorityNormal3      = 24 + 3, ///< Priority: normal + 3
    TaskPriorityNormal4      = 24 + 4, ///< Priority: normal + 4
    TaskPriorityNormal5      = 24 + 5, ///< Priority: normal + 5
    TaskPriorityNormal6      = 24 + 6, ///< Priority: normal + 6
    TaskPriorityNormal7      = 24 + 7, ///< Priority: normal + 7
    TaskPriorityAboveNormal  = 32,     ///< Priority: above normal
    TaskPriorityAboveNormal1 = 32 + 1, ///< Priority: above normal + 1
    TaskPriorityAboveNormal2 = 32 + 2, ///< Priority: above normal + 2
    TaskPriorityAboveNormal3 = 32 + 3, ///< Priority: above normal + 3
    TaskPriorityAboveNormal4 = 32 + 4, ///< Priority: above normal + 4
    TaskPriorityAboveNormal5 = 32 + 5, ///< Priority: above normal + 5
    TaskPriorityAboveNormal6 = 32 + 6, ///< Priority: above normal + 6
    TaskPriorityAboveNormal7 = 32 + 7, ///< Priority: above normal + 7
    TaskPriorityHigh         = 40,     ///< Priority: high
    TaskPriorityHigh1        = 40 + 1, ///< Priority: high + 1
    TaskPriorityHigh2        = 40 + 2, ///< Priority: high + 2
    TaskPriorityHigh3        = 40 + 3, ///< Priority: high + 3
    TaskPriorityHigh4        = 40 + 4, ///< Priority: high + 4
    TaskPriorityHigh5        = 40 + 5, ///< Priority: high + 5
    TaskPriorityHigh6        = 40 + 6, ///< Priority: high + 6
    TaskPriorityHigh7        = 40 + 7, ///< Priority: high + 7
    TaskPriorityRealtime     = 48,     ///< Priority: realtime
    TaskPriorityRealtime1    = 48 + 1, ///< Priority: realtime + 1
    TaskPriorityRealtime2    = 48 + 2, ///< Priority: realtime + 2
    TaskPriorityRealtime3    = 48 + 3, ///< Priority: realtime + 3
    TaskPriorityRealtime4    = 48 + 4, ///< Priority: realtime + 4
    TaskPriorityRealtime5    = 48 + 5, ///< Priority: realtime + 5
    TaskPriorityRealtime6    = 48 + 6, ///< Priority: realtime + 6
    TaskPriorityRealtime7    = 48 + 7, ///< Priority: realtime + 7
    TaskPriorityISR          = 56,     ///< Reserved for ISR deferred thread.
    TaskPriorityError = -1, ///< System cannot determine priority or illegal
                            ///< priority.
    TaskPriorityReserved = 0x7FFFFFFF ///< Prevents enum down-size compiler
};

//! @}
//! @defgroup Depscan_General_Typedefs
//! @{

//! @brief System generic time unit. Can hold 2^64 - 1 microseconds.
typedef uint64_t usec_t;

//! @brief Timer adapter structure compatible with logical timer classes.
typedef struct
{
    uint64_t data_[2];
} timer_handle_t;

//! @}
//! @addtogroup Depscan_Debug
//! @{

#define debug_trace( fmt, ... )                                                \
    {                                                                          \
        void API_Msgf( const char*, ... );                                     \
        API_Msgf( fmt, __VA_ARGS__ );                                          \
    }

//! @}
//! @}
//! @}
