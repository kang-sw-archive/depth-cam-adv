#pragma once
#include <FreeRTOS.h>

#include <argus.h>
#include <stdbool.h>
#include <stdint.h>
#include "../defs.h"
#include "../protocol/protocol-s.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//! @addtogroup Depscan
//! @{
//! @addtogroup Depscan_Capture
//! @{
typedef struct
{
    float x;
    float y;
} capture_fpoint_t;

typedef struct
{
    int x;
    int y;
} capture_point_t;

typedef enum
{
    CAPTURE_MODE_NONE,
    CAPTURE_MODE_LINE,
    CAPTURE_MODE_POINT
} capture_mode_t;

//! @brief      A struct indicates common capture status
typedef struct capture_common_ty__
{
    //! @brief      Sensor properties
    argus_hnd_t* SensorHandle;

    //! @brief      Motor properties
    capture_fpoint_t AnglePerStep;

    //! @brief      Scanner props
    //! @{
    capture_point_t Scan_Resolution;
    capture_point_t Scan_StepPerPxl;
    capture_point_t Scan_CaptureOfst;
    capture_point_t Scan_Pos;
    //! @}

    //! @brief      Point properties
    //! @{
    int      Point_NumMaxRequest;
    int      Point_NumPendingRequest;
    uint32_t Point_RequestID;
    uint32_t Point_BufHead;
    uint32_t Point_BufEnd;
    //! @}

    //! @brief      Process handle
    TaskHandle_t CaptureProcess;
    bool         bPaused;

    //! @brief      Data buffer for general usage
    char Buffer[CAPTURER_BUFFER_SIZE];
} capture_t;

void Capture_Point_Begin( void* ARG );

//! @}
//! @}
#ifdef __cplusplus
}
#endif // __cplusplus