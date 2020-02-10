/*************************************************************************/

/**
 * @file
 * @brief    	This file is part of the
 *Argus API.
 * @details		This file provides an
 *interface for the required timer modules.
 *
 * @copyright	Copyright c 2016-2018, Avago
 *Technologies GmbH. All rights reserved.
 *****************************************************************************/

/*! ***************************************************************************
 * @defgroup	argus_timer Timer: Argus Timer Interface
 * @ingroup		argus_platform
 * @brief 		Timer implementations for lifetime counting as well as periodic
 * 				callback.
 * @details		The module provides an interface to the timing utilities that
 * 				are required by the Argus time-of-flight sensor API.
 *
 *				Two essential features have to be provided by the user code:
 *				1. Time Measurement Capability: In order to keep track of
 *outgoing signals, the API needs to measure elapsed time. In order to provide
 *optimum device performance, the granularity should be in the microseconds
 *regime.
 *				2. Periodic Callback: The API provides an automatic starting of
 *				  measurement cycles at a fixed frame rate via a periodic
 *				  interrupt timer. If this feature is not used, implementation
 *				  of the periodic interrupts can be skipped.
 *				.
 *
 *				The time measurement feature is simply implemented by the
 *function #Timer_GetCounterValue. Whenever the function is called, the provided
 *counter values must be written with the values obtained by the current time.
 *
 *				The periodic interrupt timer is a simple callback interface.
 *				After installing the callback function pointer via
 *#Timer_SetCallback, the timer can be started by setting interval via
 *#Timer_SetInterval or #Timer_Start. From then, the callback is invoked
 *periodically as the corresponding interval may specify. The timer is stopped
 *via #Timer_Stop or by setting the interval to 0. The interval can be updated
 *at any time by updating the interval via the #Timer_SetInterval function. To
 *any of these functions, an abstract parameter pointer must be passed. This
 *parameter is passed back to the callback any time it is invoked.
 *
 *				In order to provide the usage of multiple devices, an mechanism
 *is introduced to allow the installation of multiple callback interval at the
 *same time. Therefore, the abstract parameter pointer is used to identify the
 *corresponding callback interval. For example, there are two callbacks for two
 *intervals, t1 and t2, required. The user can start two timers by calling the
 *#Timer_Start method twice, but with an individual parameter pointer, ptr1 and
 *ptr2, each: \code Timer_Start(100000, ptr1); // 10 ms callback w/ parameter
 *ptr1 Timer_Start(200000, ptr2); // 20 ms callback w/ parameter ptr1 \endcode
 *
 *				Note that the implemented timer module must therefore support
 *				as many different intervals as instanced of the Argus API are
 *				used.
 *
 * @addtogroup 	argus_timer
 * @{
 *****************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include <tuple>
#include "../../app/app.h"

static std::pair<timer_handle_t, void*> s_h[4];

extern "C" {
typedef void ( *timer_cb_t )( void* param );
static timer_cb_t s_cb;

void Timer_GetCounterValue( uint32_t* hct, uint32_t* lct )
{
    auto t = API_GetTime_us();
    *hct   = t / (uint32_t)1e6;
    *lct   = t % (uint32_t)1e6;
}

void Timer_SetCallback( timer_cb_t f )
{
    s_cb = f;
}

void Timer_Stop( void* param )
{
    for ( auto& h : s_h )
    {
        if ( h.second == param )
        {
            API_AbortTimer( h.first );
            h.second = NULL;
            break;
        }
    }
}

void Timer_SetInterval( uint32_t dt_microseconds, void* param )
{
    // Try abort
    Timer_Stop( param );

    // Check if timer's running
    for ( auto& h : s_h )
    {
        if ( API_CheckTimer( h.first, NULL ) == false )
        {
            h.first  = API_SetTimer( dt_microseconds, param, s_cb );
            h.second = param;
            break;
        }
    }
}

void Timer_Start( uint32_t dt_microseconds, void* param )
{
    Timer_SetInterval( dt_microseconds, param );
}
}