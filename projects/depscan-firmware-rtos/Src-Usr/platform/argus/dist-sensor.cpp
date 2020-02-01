#include "..\..\app\dist-sensor.h"

//! Static sensor instance
static struct dist_sens__
{
} s_inst;

//! Exported distance sensor handle
dist_sens_t ghDistSens = &s_inst;

extern "C" {
void DistSens_Configure( dist_sens_t, dist_sens_config_t const* opt )
{ }

void DistSens_GetConfigure( dist_sens_t, dist_sens_config_t* out )
{ }

void DistSens_GetDistanceFxp( dist_sens_t, fxp9_22_t* out )
{ }

bool DistSens_MeasureSync( dist_sens_t, uint32_t Retry )
{
    return false;
}

bool DistSens_MeasureAsync( dist_sens_t, uint32_t Retry, void* cb_obj, dist_sens_async_cb_t cb )
{
    return false;
}
}