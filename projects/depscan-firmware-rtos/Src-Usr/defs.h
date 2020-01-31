#pragma once
#include <stdint.h>

#define USB_READ_BUF_SIZE  1024
#define USB_WRITE_BUF_SIZE 1024

#define HOST_TRANSFER_BUFFER_SIZE 0x1000

#define NUM_MAX_HWTIMER_NODE 40
#define NUM_TIMER_TASK_STACK_WORDS 512
#define NUM_MAX_EXPORT_BINARY 40

typedef uint64_t usec_t;

typedef struct {
    uint64_t data_[2];
} timer_handle_t;
