#pragma once
#include <stdint.h>

#define USB_READ_BUF_SIZE  1024
#define USB_WRITE_BUF_SIZE 1024

#define HOST_TRANSFER_BUFFER_SIZE 0x1000

#define NUM_MAX_HWTIMER_NODE 40

#define NUM_TIMER_TASK_STACK_WORDS 512

typedef uint64_t usec_t;
typedef uint64_t timer_handle_t;

