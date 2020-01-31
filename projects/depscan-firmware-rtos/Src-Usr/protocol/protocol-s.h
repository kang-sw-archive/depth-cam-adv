//! @brief
//!
//! @file scanner_protocol.h
//! @author Seungwoo Kang (ki6080@gmail.com)
//! @date 2019-12-26
//! @copyright Copyright (c) 2019. Seungwoo Kang. All rights reserved.
//!
//! @details
#pragma once
#include <stdint.h>

#pragma pack( push, 2 )

typedef uint16_t SCANNER_COMMAND_TYPE;

typedef struct {
    uint16_t StepPerPxlX;
    uint16_t StepPerPxlY;
    uint32_t SizeX;
    uint32_t SizeY;
    uint32_t OfstX;
    uint32_t OfstY;
    uint32_t DelayPerCapture;

    float DegreePerStepX;
    float DegreePerStepY;

    int32_t CurMotorStepX;
    int32_t CurMotorStepY;

    int64_t TimeAfterLaunch_us;

    bool bIsPrecisionMode;
    bool bIsIdle;
    bool bIsSensorInitialized;
    bool bIsPaused;
} FDeviceStat;

enum ECaptureModeBit {
    ECMB_DISTANCE_AUTO = 1,
    ECMB_DISTANCE_FAR  = 2,
    ECMB_LOW_PRECISION = 4,
};

typedef int32_t  q9_22_t;
typedef uint16_t uq12_4_t;
#define Q9_22_ONE_INT  ( 1 << 22 )
#define UQ12_4_ONE_INT ( 1u << 4 )

#define SCANNER_NUM_GET_TAG_LENGTH 20

typedef struct {
    q9_22_t  Distance;
    uq12_4_t AMP;
} FPxlData;

typedef struct {
    uint32_t LineIdx;
    uint32_t OfstX;
    uint32_t NumPxls;
} FLineDdesc;

typedef struct {
    int16_t  X;     //!< Offset steps from initial point
    int16_t  Y;     //!< Offset steps from initial point
    FPxlData V;     //!< Actual distance value
    uint16_t pad__; //!< 4Byte align
} FPointData;

typedef struct {
    int16_t X;
    int16_t Y;
} FPointReq;

typedef struct {
    uint32_t ID;        //!< Simple 32bit value to identify request
    uint32_t IndexOfst; //!< Number of points sent from initial request
    uint32_t NumPoints; //!< Number of points in current transfer
} FPointSetDesc;

typedef struct {
    char   TAG[SCANNER_NUM_GET_TAG_LENGTH];
    size_t Length;
} FGetDesc;

typedef FPointSetDesc FPointReqSetDesc;

#ifdef __cplusplus
namespace ECommand {
#endif
enum ECommand // REQ = HOST -> DEVICE, RSP = DEVICE -> HOST
{
    REQ_POINT_SET,

    RSP_STAT_REPORT,

    RSP_PING,

    RSP_LINE_DATA,
    RSP_PIXEL_DATA,
    RSP_DONE,

    RSP_POINT_SET,
    RSP_POINT,

    RSP_GET
};
#ifdef __cplusplus
}
#endif

#pragma pack( pop )
