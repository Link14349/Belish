#include "timestamp.h"

#ifdef I_OS_WIN
#include <windows.h>

int64_t getCurrentTime() {
    SYSTEMTIME wtm;
    GetLocalTime(&wtm);
    return wtm.wSecond * 1000LL;
}
#else

int64_t getCurrentTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
#endif
