#pragma once
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

//! @addtogroup Depscan
//! @{
//! @defgroup Depscan_DistanceSensor
//! @brief Provides a platform independent interface for handling distance
//!     sensors.
//! @details
//!      Basically, the distance sensor interface was designed considering the
//!     ARGUS API, which is the driver of the AFBR-S50 sensor used in this
//!     project. \n
//!      Instead of using the ARGUS API as it is, the module utilizes a more
//!     abstracted adapter to increase reusability and to be more flexible in
//!     future sensor hardware changes. \n
//!      Hardware management is entirely up to the implementation layer, so
//!     this module does not contain any logic for creating handles. Handles
//!     must be created at program startup and valid at the time of use.
//! @{

//! @brief Sensor handle typedef
typedef struct dist_sens__* dist_sens_t;

//! @brief Sensor configuration descriptor
typedef struct dist_sens_config__
{
    uint16_t Delay_us;           //!< Delays per capture
    bool     bCloseDistanceMode; //!< Distance mode. True if close distance &
                                 //!< accuracy mode
} dist_sens_config_t;

//! @brief Constants
//! @{
#define DIST_SENS_RETRY_MAX ( (uint32_t)-1 )
#define DIST_SENS_OK        0
//! @}

//! @brief Fixed pointer. Integers 9 bits, Fractals 22 bits
typedef int32_t fxp9_22_t;

//! @brief Unsigned fixed pointer. Integers 12 bits, fractals 4 bits.
typedef uint16_t ufxp_12_4_t;

//! @brief Asynchronous measurement notifier callback type.
typedef void (
    *dist_sens_async_cb_t )( dist_sens_t, void* /*obj*/, int /* error_code */ );

//! @brief Configure sensor with given configuration properties
void DistSens_Configure( dist_sens_t, dist_sens_config_t const* opt );

//! @param Get configuration info from sensor
void DistSens_GetConfigure( dist_sens_t, dist_sens_config_t* out );

//! @brief Get measurement result value as FXP9_22_t.
//! @param [out] out Get measurement result in fxp format
void DistSens_GetDistanceFxp( dist_sens_t, fxp9_22_t* out );

//! @brief Get amplitude value as UFXP4_8_t
void DistSens_GetDistanceFxp( dist_sens_t, fxp9_22_t* out );

//! @brief Trigger measurement synchronously
//! @param Retry Number of retry chance when measurement fails. \n
//!      Specify DIST_SENS_RETRY_MAX to retry until succeed. Actual retry time
//!     is associated with Delay_us in @ref dist_sens_config_t.
//! @returns false when all retry exhausted.
bool DistSens_MeasureSync( dist_sens_t, uint32_t Retry );

//! @brief Trigger measurement asynchronously
//! @param Retry See \ref DistSens_MeasureSync
//! @param cb_obj Callback object bound to cb
//! @param cb Callback call when measurement done.
//! @returns See \ref DistSens_MeasureSync
//! @see DistSens_MeasureSync
bool DistSens_MeasureAsync(
    dist_sens_t,
    uint32_t             Retry,
    void*                cb_obj,
    dist_sens_async_cb_t cb );

//! @}
//! @}

#ifdef __cplusplus
}
#endif // __cplusplus