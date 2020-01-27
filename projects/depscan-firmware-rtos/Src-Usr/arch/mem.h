#pragma once
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

static inline void* AppMalloc( size_t sz )
{
    void* pvPortMalloc( size_t sz );
    return pvPortMalloc( sz );
}

static inline void AppFree( void* ptr )
{
    void vPortFree( void* );
    vPortFree( ptr );
}

#ifdef __cplusplus
}
#endif // __cplusplus