/** ------------------------------- **
   @brief Argus HW timer logic implementation
   
   @file agtimer.c
   @author Seungwoo Kang (ki6080@gmail.com)
   @version 0.1
   @date 2019-12-24
   @copyright Copyright (c) 2019. Seungwoo Kang. All rights reserved.
   
   @details
 **/
#include <uEmbedded/uassert.h>

#include <platform/argus_timer.h>

#include <app/program.h>

#include <arch/hw-timer.h>
#include <arch/irq.h>
#define print(...)

/*************************************************************************/ /**
 * @file
 * @brief    	This file is part of the Argus API.
 * @details		This file provides an interface for the required timer modules.
 *
 * @copyright	Copyright c 2016-2018, Avago Technologies GmbH.
 * 				All rights reserved.
 *****************************************************************************/
// #define print(...)
//=====================================================================//
//
// <+> Static variables.
//
//=====================================================================//
static timer_cb_t g_cbTimer;

/*! \breif      Timer pair struct that describes timer handle as random pointer value. 
    \details    The first element of pair array is fixed value of NULL parameter. */
static struct timer_param_hnd_pair {
    void*          param;
    timer_handle_t hnd;
} g_param_hnd_pairs[8];

//=====================================================================//
//
// <+> Function Implementations
//
//=====================================================================//
#define countof(v) (sizeof(v) / sizeof(*v))

static inline struct timer_param_hnd_pair* findPair(void* param)
{
    size_t i;
    if (param == NULL) {
        return &g_param_hnd_pairs[0];
    }
    for (i = 1; i < countof(g_param_hnd_pairs) && g_param_hnd_pairs[i].param; i++) {
        if (g_param_hnd_pairs[i].param == param)
            return &g_param_hnd_pairs[i];
    }
    return i == countof(g_param_hnd_pairs) ? NULL : g_param_hnd_pairs + i;
}

static inline void removePair(struct timer_param_hnd_pair* pair)
{
    uassert(pair);

    typedef struct timer_param_hnd_pair pair_t;
    pair_t*                             end  = g_param_hnd_pairs + countof(g_param_hnd_pairs) - 1;
    pair_t*                             head = pair;

    // Find last active param
    for (; head != end && head->param; ++head)
        ;

    // Draw last element to removing element, and remove element at last.
    *pair       = *head;
    head->param = NULL;
}

void Timer_GetCounterValue(uint32_t* hct, uint32_t* lct)
{
    timetick_t t = GetTime();
    *hct         = t.ms / 1000;
    *lct         = (t.ms % 1000) * 1000 + t.us;
}

void Timer_SetCallback(timer_cb_t f)
{
    print("agTimer callback settled.\n");
    g_cbTimer = f;
}

static void timerCallback(void* param)
{
    struct timer_param_hnd_pair* pair = findPair(param);
    timer_info_t const*          info = timer_browse(&g_timer, pair->hnd); 
    removePair(pair);

    g_cbTimer(param);
}

void Timer_SetInterval(uint32_t dt_microseconds, void* param)
{
    // print( "agTimer Set interval called \n" );
    struct timer_param_hnd_pair* s = findPair(param);
    uassert(s != NULL);

    if (param == NULL || s->param) {
        AbortEventTimer(s->hnd);
    }
    else {
        s->param = param;
    }

    print("Queueing timer for %x\n", param);
    s->hnd = QueueEventTimer(timerCallback, param, dt_microseconds - 1);
}

void Timer_Start(uint32_t dt_microseconds, void* param)
{
    Timer_SetInterval(dt_microseconds, param);
}

void Timer_Stop(void* param)
{
    // print( "agTimer Stop called. But does nothing. \n" );
    struct timer_param_hnd_pair* p = findPair(param);
    if (!p)
        return;

    AbortEventTimer(p->hnd);
    removePair(p);
}
