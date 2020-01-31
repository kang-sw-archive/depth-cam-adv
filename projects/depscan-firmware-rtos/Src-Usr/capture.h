#pragma once
#include <argus.h>
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
struct capture_common_ty__ {
    capture_fpoint_t AnglePerStep;
};

//! A struct indicates line capture status
typedef struct {
    capture_common_ty__ stat;
} capture_line_stat_t;

//! A struct indicates point capture status
typedef struct {
    capture_common_ty__ stat;
} capture_point_stat_t;

#ifdef __cplusplus
}
#endif // __cplusplus