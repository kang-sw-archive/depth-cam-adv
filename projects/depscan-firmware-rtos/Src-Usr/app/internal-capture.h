#pragma once
#include <FreeRTOS.h>

#include <argus.h>
#include <stdbool.h>
#include <stdint.h>
#include <task.h>
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
//! @details
//!              This class will be used as singleton instance of capturing
//!             condition.
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
    capture_point_t Scan_Pos; //< This is for legacy protocol compatibility
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
    TaskHandle_t   CaptureTask;
    capture_mode_t CurrentMode;
    bool           bPaused;
    bool           bPendingStop;

    //! @brief      Data buffer for general usage
} capture_t;

//! @brief      Singleton instance of capturing status.
extern capture_t gCapture;

//! @brief      Buffer.
//! @note
//!              This is detached from capture_t descriptor to reduce .data
//!             section usage
extern char Capture_Buffer[CAPTURER_BUFFER_SIZE];

//! @brief      Check if capturing process is in progress
static inline bool Capture_IsRunning()
{
    return gCapture.CaptureTask != NULL;
}

//! @}
//! @}
#ifdef __cplusplus
}
#endif // __cplusplus