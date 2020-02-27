#ifndef BELISH_TIMESTAMP_H
#define BELISH_TIMESTAMP_H

#include "../OSPlatformUtil/src/osplatformutil.h"

#if !defined(I_OS_WIN)
#include <sys/time.h>
#endif

#include <cstdlib>
#include "trans.h"

int64_t getCurrentTime();

#endif //BELISH_TIMESTAMP_H
