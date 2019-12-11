#pragma once

#include <stdio.h>
#include "../platform/PlatformMacros.h"


/** @def BCX_FORMAT_PRINTF(formatPos, argPos)
 * Only certain compiler support __attribute__((format))
 *
 * @param formatPos 1-based position of format string argument.
 * @param argPos    1-based position of first format-dependent argument.
 */
#if defined(__GNUC__) && (__GNUC__ >= 4)
#define BCX_FORMAT_PRINTF(formatPos, argPos) __attribute__((__format__(printf, formatPos, argPos)))
#elif defined(__has_attribute)
#if __has_attribute(format)
#define BCX_FORMAT_PRINTF(formatPos, argPos) __attribute__((__format__(printf, formatPos, argPos)))
#endif // __has_attribute(format)
#else
#define BCX_FORMAT_PRINTF(formatPos, argPos)
#endif

#if (BCX_TARGET_PLATFORM == BCX_PLATFORM_WIN32)
#define BCXLOG(fmt, ...) bcx::log(("%s [Line %d] " fmt), __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define BLog(fmt, ...) bcx::log(("%s [Line %d] " fmt), __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define BCXLOG(fmt, ...) bcx::log(("%s [Line %d] " fmt), __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__)
#define BLog(fmt, ...) bcx::log(("%s [Line %d] " fmt), __FUNCTION__, __LINE__, ##__VA_ARGS__)
#endif

namespace bcx {
    /// The max length of CCLog message.
    static const int MAX_LOG_LENGTH = 16*1024;

    void log(const char * format, ...) BCX_FORMAT_PRINTF(1, 2);
}

