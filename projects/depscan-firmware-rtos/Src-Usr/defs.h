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
#define HOST_TRANSFER_BUFFER_SIZE  0x800
#define NUM_MAX_HWTIMER_NODE       20
#define NUM_TIMER_TASK_STACK_WORDS 768
#define NUM_MAX_EXPORT_BINARY      40
#define CAPTURER_BUFFER_SIZE       2048

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
