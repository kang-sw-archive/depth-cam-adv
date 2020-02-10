//! @file
//! @brief
//!
//! @author Seungwoo Kang (ki6080@gmail.com)
//! @copyright Copyright (c) 2019. Seungwoo Kang. All rights reserved.
//!
//! @details
//! @todo Check if default calibration is required or not
#include <FreeRTOS.h>

#include <argus.h>
#include <semphr.h>
#include <uEmbedded/uassert.h>
#include "../../app/app.h"
#include "../../app/dist-sensor.h"
#include "../../platform/s2pi.h"

/////////////////////////////////////////////////////////////////////////////
// Types
//! Static sensor instance
static struct dist_sens__
{
    argus_hnd_t*       hnd_          = NULL;
    bool               init_correct_ = false;
    bool               capturing_    = false;
    bool               valid_data    = false;
    dist_sens_config_t conf_
      = { .Delay_us = 1000u, .bCloseDistanceMode = true };

    void*                cb_obj_ = NULL;
    dist_sens_async_cb_t cb_     = NULL;

    timer_handle_t watchdog_hnd_ = {};

    argus_results_t result_;

} si;

//! Exported distance sensor handle
dist_sens_t ghDistSens = &si;

static bool RefreshArgusSens();

/////////////////////////////////////////////////////////////////////////////
// Defs
extern "C" {
bool DistSens_IsAvailable( dist_sens_t h )
{
    return h->init_correct_ && !h->capturing_
           && Argus_GetStatus( h->hnd_ ) == STATUS_OK;
}

bool DistSens_Configure( dist_sens_t h, dist_sens_config_t const* opt )
{
    if ( h->capturing_ )
    {
        API_Msg( "error: Cannot configure sensor during capture \n" );
        return false;
    }

    if ( RefreshArgusSens() == false )
    {
        API_Msg( "error: Configuration failed.\n" );
        return false;
    }

    if ( opt == nullptr )
        opt = &h->conf_;

    status_t res_f = Argus_SetConfigurationFrameTime( h->hnd_, opt->Delay_us ),
             res_m = Argus_SetConfigurationMeasurementMode(
               h->hnd_, opt->bCloseDistanceMode ? ARGUS_MODE_B : ARGUS_MODE_A );

    if ( res_f == STATUS_OK )
        h->conf_.Delay_us = opt->Delay_us;
    else
        API_Msg( "Failed to configure frame time\n" );

    if ( res_m == STATUS_OK )
        h->conf_.bCloseDistanceMode = opt->bCloseDistanceMode;
    else
        API_Msg( "Failed to configure distance mode\n" );

    return res_f == STATUS_OK && res_m == STATUS_OK;
}

void DistSens_GetConfigure( dist_sens_t h, dist_sens_config_t* out )
{
    *out = h->conf_;
}

int DistSens_MeasureSync( dist_sens_t h, uint32_t Retry )
{
    volatile struct cb_param_type
    {
        TaskHandle_t tid    = xTaskGetCurrentTaskHandle();
        int          result = ERROR_FAIL;
    } cb_param;

    dist_sens_async_cb_t const cb = []( dist_sens_t, void* obj, int res ) {
        auto param    = (volatile cb_param_type*)obj;
        param->result = res;
        xTaskNotifyGive( param->tid );
    };

    if ( DistSens_MeasureAsync( h, Retry, (void*)&cb_param, cb ) == false )
        return false;

    ulTaskNotifyTake( pdTRUE, (TickType_t)-1 );
    return cb_param.result;
}

bool DistSens_MeasureAsync(
  dist_sens_t          nouse_,
  uint32_t             Retry,
  void*                cb_obj,
  dist_sens_async_cb_t cb )
{
    if ( !si.init_correct_ )
    {
        API_Msg( "error: sensor is not initialized correctly.\n" );
        return false;
    }
    if ( si.capturing_ )
    {
        API_Msg( "error: measurement already triggered.\n" );
        return false;
    }
    si.capturing_ = true;  // Prevent other task requesting capture
    si.valid_data = false; // Invalidate previous measurement
    si.cb_        = cb;
    si.cb_obj_    = cb_obj;

    // Trigger measurement
    const argus_callback_t cb_a = []( status_t result, void* raw ) -> status_t {
        if ( result == STATUS_OK )
        {
            result = Argus_EvaluateData(
              si.hnd_, &si.result_, (ads_value_buf_t*)raw );

            // Both of evaluation and data status should be OK
            if ( result == STATUS_OK )
                result = si.result_.Status;

            if ( result < STATUS_OK )
            { // When the result is negative, it indicates an error.
                API_Msgf(
                  "error: failed to evaluate data for code %d\n", result );
                si.init_correct_ = false;
            }
            else
            { // Where the result is a positive value, the data is present even
              // its validity is not guaranteed. Handling non-zero result is
              // mandated to the callback.
                si.valid_data = true;
            }
        }
        else
        {
            // Invalidate sensor status
            si.init_correct_ = false;
            API_Msgf( "error: failed to measure data for code %d\n", result );
        }

        // Deactivate watchdog timer.
        API_AbortTimer( si.watchdog_hnd_ );

        // Callback is always invoked regardless of the measurement result
        si.capturing_ = false;
        if ( si.cb_ )
            si.cb_( ghDistSens, si.cb_obj_, result );
        return result;
    };

    for ( size_t i = 0; i < Retry; )
    {
        status_t result = Argus_TriggerMeasurement( si.hnd_, cb_a );

        // Power-limit does not consume retry.
        if ( result == STATUS_ARGUS_POWERLIMIT )
        {
            taskYIELD();
            continue;
        }

        if ( result == STATUS_OK )
        {
            // To prevent waiting forever, a watchdog timer will trigger to
            // reset sensor's status when the sensor doesn't respond.
            si.watchdog_hnd_ = API_SetTimer(
              si.conf_.Delay_us * 4, NULL, []( auto ) {
                  API_Msg( "warning: Oops, seems capture request is lost! \n" );
                  si.capturing_    = false;
                  si.init_correct_ = false;
              } );
            return true;
        }

        // If failed ...
        switch ( result )
        {
        case STATUS_BUSY:
            // Argus_Abort( si.hnd_ );
            break;
        case ERROR_ABORTED:
            RefreshArgusSens();
            DistSens_Configure( ghDistSens, &si.conf_ );
            break;
        default:
            break;
        }
        ++i;
    }

    // All retries consumed ...
    si.capturing_    = false;
    si.init_correct_ = false;
    return false;
}

bool DistSens_GetAmpFxp( dist_sens_t, ufxp_12_4_t* out )
{
    if ( si.capturing_ || !si.valid_data )
        return false;

    *out = si.result_.Bin.Amplitude;
    return true;
}

bool DistSens_GetDistanceFxp( dist_sens_t, fxp9_22_t* out )
{
    if ( si.capturing_ || !si.valid_data )
        return false;

    *out = si.result_.Bin.Range;
    return true;
}
}

/////////////////////////////////////////////////////////////////////////////
// Redirects
void* Argus_Malloc( size_t size )
{
    return pvPortMalloc( size );
}

void Argus_Free( void* ptr )
{
    vPortFree( ptr );
}

static bool RefreshArgusSens()
{
    // Aliasing
    auto& s = si;

    if ( s.init_correct_ )
        return true;

    // Use 'capturing' flag as semaphore to prevent other process accessing
    // handle
    uassert( si.capturing_ == false );
    si.capturing_ = true;

    // Allocate memory if not exists
    if ( s.hnd_ == NULL )
        s.hnd_ = Argus_CreateHandle();

    auto res = Argus_Init( s.hnd_, S2PI_SLAVE_ARGUS );
    if ( res == STATUS_OK )
    {
        // Configure calibration options of sensor only when initialization
        // process is finished without error. Sensor will be configured with
        // default calibrations
        argus_calibration_t calib;
        Argus_GetDefaultCalibration( &calib );

        // Stores result to evaluate configuration result.
        res = Argus_SetCalibration( s.hnd_, &calib );

        Argus_SetConfigurationFrameTime( s.hnd_, s.conf_.Delay_us );
        Argus_SetConfigurationMeasurementMode(
          s.hnd_, s.conf_.bCloseDistanceMode ? ARGUS_MODE_B : ARGUS_MODE_A );
    }

    si.capturing_ = false;
    if ( res != STATUS_OK )
    {
        API_Msgf(
          "error: Failed to initialize ARGUS library. exit code: %d \n", res );
        return false;
    }

    s.init_correct_ = true;
    return true;
}
