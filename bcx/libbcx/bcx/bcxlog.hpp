//
//  bcxlog.hpp
//  bcx
//
//  Created by admin on 2019/3/5.
//  Copyright © 2019 SDKBox. All rights reserved.
//

#ifndef bcxlog_hpp
#define bcxlog_hpp

#include <stdio.h>

/** @def CC_FORMAT_PRINTF(formatPos, argPos)
 * Only certain compiler support __attribute__((format))
 *
 * @param formatPos 1-based position of format string argument.
 * @param argPos    1-based position of first format-dependent argument.
 */
#if defined(__GNUC__) && (__GNUC__ >= 4)
#define CC_FORMAT_PRINTF(formatPos, argPos) __attribute__((__format__(printf, formatPos, argPos)))
#elif defined(__has_attribute)
#if __has_attribute(format)
#define CC_FORMAT_PRINTF(formatPos, argPos) __attribute__((__format__(printf, formatPos, argPos)))
#endif // __has_attribute(format)
#else
#define CC_FORMAT_PRINTF(formatPos, argPos)
#endif

#define BCXLOG(fmt, ...) bcx::log(("%s [Line %d] " fmt), __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__)

namespace bcx {
    /// The max length of CCLog message.
    static const int MAX_LOG_LENGTH = 16*1024;
    /**
     @brief Output Debug message.
     */
    void log(const char * format, ...) CC_FORMAT_PRINTF(1, 2);

}


#endif /* bcxlog_hpp */
