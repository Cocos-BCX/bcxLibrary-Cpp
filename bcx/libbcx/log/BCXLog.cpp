#include "BCXLog.hpp"

#if (BCX_TARGET_PLATFORM == BCX_PLATFORM_ANDROID)
#include <android/log.h>
#elif (BCX_TARGET_PLATFORM == BCX_PLATFORM_WIN32)
#include <stdarg.h>
#endif

#include <string>
#include <cstring>

namespace bcx {

    static void _log(const char *format, va_list args) {
        int bufferSize = MAX_LOG_LENGTH;
        char* buf = nullptr;
        
        do {
            buf = new (std::nothrow) char[bufferSize];
            if (buf == nullptr)
                return; // not enough memory
            
            int ret = vsnprintf(buf, bufferSize - 3, format, args);
            if (ret < 0) {
                bufferSize *= 2;
                
                delete [] buf;
            } else {
                break;
            }
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
    
    void log(const char * format, ...) {
        va_list args;
        va_start(args, format);
        _log(format, args);
        va_end(args);
    }

}

