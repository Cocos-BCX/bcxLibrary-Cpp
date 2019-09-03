#ifndef __PLATFORM_MACROS_H__
#define __PLATFORM_MACROS_H__

#include "./PlatformDefine.h"


/// @name namespace cocos2d
/// @{
#ifdef __cplusplus
    #define NS_BCX_BEGIN                     namespace bcx {
    #define NS_BCX_END                       }
    #define USING_NS_BCX                     using namespace bcx
    #define NS_BCX                           ::bcx
#else
    #define NS_BCX_BEGIN
    #define NS_BCX_END
    #define USING_NS_BCX
    #define NS_BCX
#endif 
//  end of namespace group
/// @}


#define BCX_SAFE_DELETE(p)           do { delete (p); (p) = nullptr; } while(0)
#define BCX_SAFE_DELETE_ARRAY(p)     do { if(p) { delete[] (p); (p) = nullptr; } } while(0)
#define BCX_SAFE_FREE(p)             do { if(p) { free(p); (p) = nullptr; } } while(0)
#define BCX_SAFE_RELEASE(p)          do { if(p) { (p)->release(); } } while(0)
#define BCX_SAFE_RELEASE_NULL(p)     do { if(p) { (p)->release(); (p) = nullptr; } } while(0)
#define BCX_SAFE_RETAIN(p)           do { if(p) { (p)->retain(); } } while(0)
#define BCX_BREAK_IF(cond)           if(cond) break


/** @def BCX_DEPRECATED(...)
 * Macro to mark things deprecated as of a particular version
 * can be used with arbitrary parameters which are thrown away.
 * e.g. BCX_DEPRECATED(4.0) or BCX_DEPRECATED(4.0, "not going to need this anymore") etc.
 */
#define BCX_DEPRECATED(...) BCX_DEPRECATED_ATTRIBUTE

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

#if defined(_MSC_VER)
#define BCX_FORMAT_PRINTF_SIZE_T "%08lX"
#else
#define BCX_FORMAT_PRINTF_SIZE_T "%08zX"
#endif

#ifdef __GNUC__
#define BCX_UNUSED __attribute__ ((unused))
#else
#define BCX_UNUSED
#endif

#endif // __PLATFORM_MACROS_H__
