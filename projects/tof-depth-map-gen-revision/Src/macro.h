#pragma once

#ifdef __cplusplus
#define EXTERNC_BEGIN \
    extern "C"        \
    {
#define EXTERNC_END }


#else
#define STATIC_ASSERT(COND, MSG) typedef char static_assertion_##MSG[(!!(COND)) * 2 - 1]
// token pasting madness:
#define COMPILE_TIME_ASSERT3(X, L) STATIC_ASSERT(X, static_assertion_at_line_##L)
#define COMPILE_TIME_ASSERT2(X, L) COMPILE_TIME_ASSERT3(X, L)
#define COMPILE_TIME_ASSERT(X) COMPILE_TIME_ASSERT2(X, __LINE__)
#define static_assert__(COND, MSG) COMPILE_TIME_ASSERT(COND)

#define EXTERNC_BEGIN
#define EXTERNC_END
#endif


