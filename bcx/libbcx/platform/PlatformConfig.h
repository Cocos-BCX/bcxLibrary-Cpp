#ifndef __PLATFORM_CONFIG_H__
#define __PLATFORM_CONFIG_H__

/**
  Config of cocos2d-x project, per target platform.

  THIS FILE MUST NOT INCLUDE ANY OTHER FILE
*/

//////////////////////////////////////////////////////////////////////////
// pre configure
//////////////////////////////////////////////////////////////////////////

// define supported target platform macro which CC uses.
#define BCX_PLATFORM_UNKNOWN            0
#define BCX_PLATFORM_IOS                1
#define BCX_PLATFORM_ANDROID            2
#define BCX_PLATFORM_WIN32              3
#define BCX_PLATFORM_MARMALADE          4
#define BCX_PLATFORM_LINUX              5
#define BCX_PLATFORM_BADA               6
#define BCX_PLATFORM_BLACKBERRY         7
#define BCX_PLATFORM_MAC                8
#define BCX_PLATFORM_NACL               9
#define BCX_PLATFORM_EMSCRIPTEN        10
#define BCX_PLATFORM_TIZEN             11
#define BCX_PLATFORM_QT5               12
#define BCX_PLATFORM_WINRT             13

// Determine target platform by compile environment macro.
#define BCX_TARGET_PLATFORM             BCX_PLATFORM_UNKNOWN

// Apple: Mac and iOS
#if defined(__APPLE__) && !defined(ANDROID) // exclude android for binding generator.
    #include <TargetConditionals.h>
    #if TARGET_OS_IPHONE // TARGET_OS_IPHONE includes TARGET_OS_IOS TARGET_OS_TV and TARGET_OS_WATCH. see TargetConditionals.h
        #undef  BCX_TARGET_PLATFORM
        #define BCX_TARGET_PLATFORM         BCX_PLATFORM_IOS
    #elif TARGET_OS_MAC
        #undef  BCX_TARGET_PLATFORM
        #define BCX_TARGET_PLATFORM         BCX_PLATFORM_MAC
    #endif
#endif

// android
#if defined(ANDROID)
    #undef  BCX_TARGET_PLATFORM
    #define BCX_TARGET_PLATFORM         BCX_PLATFORM_ANDROID
#endif

// win32
#if defined(_WIN32)
    #undef  BCX_TARGET_PLATFORM
    #define BCX_TARGET_PLATFORM         BCX_PLATFORM_WIN32
#endif

// linux
#if defined(LINUX) && !defined(__APPLE__)
    #undef  BCX_TARGET_PLATFORM
    #define BCX_TARGET_PLATFORM         BCX_PLATFORM_LINUX
#endif

// marmalade
#if defined(MARMALADE)
#undef  BCX_TARGET_PLATFORM
#define BCX_TARGET_PLATFORM         BCX_PLATFORM_MARMALADE
#endif

// bada
#if defined(SHP)
#undef  BCX_TARGET_PLATFORM
#define BCX_TARGET_PLATFORM         BCX_PLATFORM_BADA
#endif

// qnx
#if defined(__QNX__)
    #undef  BCX_TARGET_PLATFORM
    #define BCX_TARGET_PLATFORM     BCX_PLATFORM_BLACKBERRY
#endif

// native client
#if defined(__native_client__)
    #undef  BCX_TARGET_PLATFORM
    #define BCX_TARGET_PLATFORM     BCX_PLATFORM_NACL
#endif

// Emscripten
#if defined(EMSCRIPTEN)
    #undef  BCX_TARGET_PLATFORM
    #define BCX_TARGET_PLATFORM     BCX_PLATFORM_EMSCRIPTEN
#endif

// tizen
#if defined(TIZEN)
    #undef  BCX_TARGET_PLATFORM
    #define BCX_TARGET_PLATFORM     BCX_PLATFORM_TIZEN
#endif

// qt5
#if defined(BCX_TARGET_QT5)
    #undef  BCX_TARGET_PLATFORM
    #define BCX_TARGET_PLATFORM     BCX_PLATFORM_QT5
#endif

// WinRT (Windows 8.1 Store/Phone App)
#if defined(WINRT)
    #undef  BCX_TARGET_PLATFORM
    #define BCX_TARGET_PLATFORM          BCX_PLATFORM_WINRT
#endif

//////////////////////////////////////////////////////////////////////////
// post configure
//////////////////////////////////////////////////////////////////////////

// check user set platform
#if ! BCX_TARGET_PLATFORM
    #error  "Cannot recognize the target platform; are you targeting an unsupported platform?"
#endif

#if (BCX_TARGET_PLATFORM == BCX_PLATFORM_WIN32)
#ifndef __MINGW32__
#pragma warning (disable:4127)
#endif
#endif  // BCX_PLATFORM_WIN32

#if ((BCX_TARGET_PLATFORM == BCX_PLATFORM_ANDROID) || (BCX_TARGET_PLATFORM == BCX_PLATFORM_IOS) || (BCX_TARGET_PLATFORM == BCX_PLATFORM_TIZEN))
    #define BCX_PLATFORM_MOBILE
#else
    #define BCX_PLATFORM_PC
#endif


#ifndef BCX_PACK_MAX_DEPTH
// The maximum level of object nesting is around 20% of this value
#define BCX_PACK_MAX_DEPTH 1000
#endif


/// @endcond
#endif  // __PLATFORM_CONFIG_H__
