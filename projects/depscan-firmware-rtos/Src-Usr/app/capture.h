#pragma once
#include <argus.h>
#include <stdbool.h>
#include <stdint.h>
#include "../defs.h"
#include "../protocol/protocol-s.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/////////////////////////////////////////////////////////////////////////////
// Definitions
typedef struct {
    float x;
    float y;
} capture_fpoint_t;

typedef struct {
    int x;
    int y;
} capture_point_t;

typedef enum {
    CAPTURE_MODE_NONE,
    CAPTURE_MODE_LINE,
    CAPTURE_MODE_POINT
} capture_mode_t;

//! A struct indicates common capture status
typedef struct capture_common_ty__ {
    //! Basic property
    capture_mode_t Mode;
    void*          Substat;

    //! Sensor properties
    argus_hnd_t* SensorHandle;
    argus_mode_t SensorMode;
    uint32_t     SensorDelay;

    //! Motor properties
    capture_fpoint_t AnglePerStep;

    //! Scanner properties

    //! Point properties
    int NumMaxRequest;

    //! Capture buffer, for general use
    char Buffer[CAPTURER_BUFFER_SIZE];
} capture_t;

/////////////////////////////////////////////////////////////////////////////
// Functions
void Capture_Point_Begin( void* ARG );
#ifdef __cplusplus
}
#endif // __cplusplus