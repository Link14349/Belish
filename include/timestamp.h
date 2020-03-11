#ifndef BELISH_TIMESTAMP_H
#define BELISH_TIMESTAMP_H

#include "../OSPlatformUtil/src/osplatformutil.h"

#if !defined(I_OS_WIN)
#include <sys/time.h>
#endif

#include <cstdlib>
#include "trans.h"

namespace Belish {
    int64_t getCurrentTime();
}
#define TIME_START(NAME) auto TIMER_START_##NAME##__ = Belish::getCurrentTime();
#define TIME_END(NAME) auto TIMER_END_##NAME##__ = Belish::getCurrentTime();
#define GET_TIME(NAME) (TIMER_END_##NAME##__ - TIMER_START_##NAME##__)

#endif //BELISH_TIMESTAMP_H
