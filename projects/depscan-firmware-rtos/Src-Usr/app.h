#pragma once
#include <cmsis_os2.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define OS_MS_TO_TICKS( ms ) ( ( ms ) / osKernelGetTickFreq() )

#ifdef __cplusplus
}
#endif // __cplusplus
