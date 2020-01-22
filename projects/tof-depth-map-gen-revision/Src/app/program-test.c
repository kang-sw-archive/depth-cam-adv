#include <stm32f4xx_hal.h>

#include "arch/irq.h"
#include "arch/transciever-usb.h"
#include "usbd_cdc_if.h"

#include "arch/hw-interface.h"
#include "program.h"
#include "protocol.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <arch/hw-timer.h>
#include <arch/s2pi-1.h>

#include <api/argus_api.h>
#include <uEmbedded/uassert.h>

extern argus_hnd_t* ghSens;

#define countof(v) (sizeof(v) / sizeof(*v))
void DistanceSensor_Initialize(void*);

static void try_capture();
static int  capture_cnt;

static status_t internal_tryCaptureDoneCallback(status_t cond, void* data);

#define TIMER_TEST_COUNT 10
struct event_timer_struct {
    int        cnt_left;
    int        interval;
    timetick_t prev;
    uint32_t   elapsed[TIMER_TEST_COUNT];
    uint32_t   gtimer_tick[TIMER_TEST_COUNT];
} event_timer_struct;

static void timer_test(void* timerval)
{
    if (--event_timer_struct.cnt_left == 0) {
        // Display all elapsed result
        for (size_t i = 0; i < TIMER_TEST_COUNT; i++) {
            print("[%02d]: For interval %d us ... \n", i, event_timer_struct.interval * i);
            print("     %d us ... error %d us ... AT GTIMER %u ticks\n",
                  event_timer_struct.elapsed[i],
                  event_timer_struct.interval * i - event_timer_struct.elapsed[i],
                  event_timer_struct.gtimer_tick[i]);
        }
        return;
    }
    timetick_t time  = GetTime();
    uint32_t   delay = time.ms - event_timer_struct.prev.ms;
    delay            = delay * 1000 + time.us - event_timer_struct.prev.us;

    event_timer_struct.elapsed[TIMER_TEST_COUNT - event_timer_struct.cnt_left]     = delay;
    event_timer_struct.gtimer_tick[TIMER_TEST_COUNT - event_timer_struct.cnt_left] = SYSTEM_TIM_TICK();
}

int TestStringHandler(int argc, char** argv)
{
    if (argc == 0)
        return 0;

    if (false && strcmp(argv[0], "init") == 0) {
        logprint("Beginning initialize sensor ... \n");
        QueueEventTimer(DistanceSensor_Initialize, NULL, 1000);

        return 1;
    }
    else if (strcmp(argv[0], "test-pattern") == 0) {
        uint8_t tx[] = {0x04, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
        uint8_t rx[sizeof(tx)];
        memset(rx, 0, sizeof(rx));

        logprint("Testing first pattern:\n\t");
        S2PI_TransferFrameSync(S2PI_SLAVE_SENSOR, tx, rx, sizeof(tx), NULL, NULL);

        for (size_t i = 0; i < sizeof(tx); i++)
            logprint("%x ", tx[i]);
        logprint("\n\t");
        for (size_t i = 0; i < sizeof(tx); i++)
            logprint("%x ", rx[i]);

        memset(tx + 1, 0, sizeof(tx) - 1);
        logprint("\nTesting second pattern:\n\t");
        S2PI_TransferFrameSync(S2PI_SLAVE_SENSOR, tx, rx, sizeof(tx), NULL, NULL);

        for (size_t i = 0; i < sizeof(tx); i++)
            logprint("%x ", tx[i]);
        logprint("\n\t");
        for (size_t i = 0; i < sizeof(tx); i++)
            logprint("%x ", rx[i]);

        memset(tx + 1, 0, 16);
        logprint("\nTesting third pattern:\n\t");
        S2PI_TransferFrameSync(S2PI_SLAVE_SENSOR, tx, rx, sizeof(tx), NULL, NULL);

        for (size_t i = 0; i < sizeof(tx); i++)
            logprint("%x ", tx[i]);
        logprint("\n\t");
        for (size_t i = 0; i < sizeof(tx); i++)
            logprint("%x ", rx[i]);

        return 1;
    }
    else if (false && strcmp(argv[0], "try-capture") == 0) {
        if (ghSens == NULL) {
            print("error: Sensor not yet initialized ! \n");
            return;
        }

        // Set measurement time
        int measure_usec = 10000;
        if (argc >= 2)
            measure_usec = atoi(argv[1]);

        print("Setting frame time by %d ...\n", measure_usec);
        status_t res = Argus_SetConfigurationFrameTime(ghSens, measure_usec);
        print("Setting frame time result = %d\n", res);
        capture_cnt = 100;
        try_capture();
        return 1;
    }
    else if (strcmp(argv[0], "test-timer") == 0) {
        int measure_usec = 10000;
        if (argc >= 2)
            measure_usec = atoi(argv[1]);

        print("Queueing timer test ... \n");
        event_timer_struct.cnt_left = 10;
        event_timer_struct.interval = measure_usec;
        event_timer_struct.prev     = GetTime();

        for (size_t i = 0; i < TIMER_TEST_COUNT; i++) {
            QueueEventTimer(timer_test, &event_timer_struct, measure_usec * (TIMER_TEST_COUNT - i));
        }
        print("Done ... \n");
        return 1;
    }
    else if (strcmp(argv[0], "timer-psc") == 0) {
        if (argc < 2)
            return 1;

        int val = atoi(argv[1]);
        print("Configure timer clock as %d\n", val);
        SYSTEM_TIM_INST->PSC = val;
        SYSTEM_TIM_INST->EGR = TIM_EGR_UG;
        return 1;
    }

    return 0;
}

//=====================================================================//
//
// <+> Callbacks
//
//=====================================================================//
static void try_capture()
{
    int retry = 10;

    for (;;) {
        status_t result = Argus_TriggerMeasurement(ghSens, internal_tryCaptureDoneCallback);

        // Wait until powerlimit state resolved.
        if (result == STATUS_ARGUS_POWERLIMIT)
            continue;

        if (result == STATUS_OK)
            break;

        if (retry--) {
            print("Retrying capture. code: %d\n", result);
            continue;
        }

        print("All retry failed. Reinitialize sensor.\n");
        uint32_t measureTime;
        Argus_GetConfigurationFrameTime(ghSens, &measureTime);
        DistanceSensor_Initialize(NULL);
        Argus_SetConfigurationFrameTime(ghSens, measureTime);
        retry = 10;
    }
}

static void internal_transmitPixelData(void* ppdata)
{
    void* data = *(void**)ppdata;

    if (data != NULL) {
        argus_results_t resData;
        status_t        EvalRes = Argus_EvaluateData(ghSens, &resData, data);

        q9_22_t  range = resData.Bin.Range;
        uq12_4_t ampl  = resData.Bin.Amplitude;

        float v1 = range / (float)Q9_22_ONE;
        float v2 = ampl / (float)(1u << 4);

        logputs("Capture Done\n");
        uint32_t measureTime;
        Argus_GetConfigurationFrameTime(ghSens, &measureTime);
        print("Measurement Time : %u\n", measureTime);
        print("Eval Result (0 is ok) : %d - data result %d\n", EvalRes, resData.Status);
        print("BIN RANGE [%f] -- AMP [%f]\n", v1, v2);
        print("RAW RANGE [%f] -- AMP [%f]\n", resData.PixelRef.Range / (float)Q9_22_ONE, resData.PixelRef.Amplitude / (float)(1u << 4));
    }

    if (capture_cnt--) {
        try_capture();
    }
}

static status_t internal_tryCaptureDoneCallback(status_t cond, void* data)
{
    // print( "Trying capture done. \n" );
    if (cond != STATUS_OK) {
        data = NULL;
    }
    // QueueEvent( &g_event, internal_transmitPixelData, &data, sizeof( void* ) );
    print("Capture done callback called. Condition = %d\n", cond);
    AppQueueEvent(internal_transmitPixelData, &data, sizeof(&data));
    // internal_transmitPixelData(data);
    return STATUS_OK;
}
