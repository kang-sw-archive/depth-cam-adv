#pragma once
#include <stdbool.h>
#include <stdint.h>

#include "defs.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct {
    float x;
    float y;
} capture_fpoint_t;

typedef struct {
    int x;
    int y;
} capture_point_t;

//! A struct indicates common capture status
typedef struct {
    capture_fpoint_t AnglePerStep;
} capture_stat_t;

//! A struct indicates line capture status
typedef struct {
} capture_line_stat_t;

//! A struct indicates

#ifdef __cplusplus
}
#endif // __cplusplus