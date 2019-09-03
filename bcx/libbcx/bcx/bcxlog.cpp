//
//  bcxlog.cpp
//  bcx
//
//  Created by admin on 2019/3/5.
//  Copyright © 2019 SDKBox. All rights reserved.
//

#include "bcxlog.hpp"

#if defined(__ANDROID__)
#include <android/log.h>
#endif // __ANDROID__

#if (BCX_TARGET_PLATFORM == BCX_PLATFORM_WIN32)
// #include <stdio.h>
#include <stdarg.h>
#endif


#include <string>
#include <cstring>

namespace bcx {

    static void _log(const char *format, va_list args)
    {
        int bufferSize = MAX_LOG_LENGTH;
        char* buf = nullptr;
        
        do
        {
            buf = new (std::nothrow) char[bufferSize];
            if (buf == nullptr)
                return; // not enough memory
            
            int ret = vsnprintf(buf, bufferSize - 3, format, args);
            if (ret < 0)
            {
                bufferSize *= 2;
                
                delete [] buf;
            }
            else
                break;
            
        } while (true);
        
        strcat(buf, "\n");
        
#if defined(__ANDROID__) //CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
        __android_log_print(ANDROID_LOG_DEBUG, "CCBCX", "%s", buf);
#else
        // Linux, Mac, iOS, etc
        fprintf(stdout, "%s", buf);
        fflush(stdout);
#endif
        
        delete [] buf;
    }
    
    void log(const char * format, ...)
    {
        va_list args;
        va_start(args, format);
        _log(format, args);
        va_end(args);
    }

    
}

