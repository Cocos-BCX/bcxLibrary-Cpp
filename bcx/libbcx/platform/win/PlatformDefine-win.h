
#ifndef __PLATFORMDEFINE_WIN_H__
#define __PLATFORMDEFINE_WIN_H__

#include <stddef.h>

#include "../PlatformConfig.h"
#if BCX_TARGET_PLATFORM == BCX_PLATFORM_WIN32

#include <assert.h>

#define BCX_DLL 

#define BCX_ASSERT(cond) assert(cond)

#define BCX_UNUSED_PARAM(unusedparam) (void)unusedparam

/* Define NULL pointer value */
#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif


#if defined(_MSC_VER)
#include <BaseTsd.h>
// typedef SSIZE_T ssize_t;
#define ssize_t SSIZE_T
#define _SSIZE_T_DEFINED
#endif


//#if defined(_MSC_VER) || defined(__MINGW32__)
//#include <BaseTsd.h>
//#include <WinSock2.h>
//
//#ifndef __SSIZE_T
//#define __SSIZE_T
//typedef SSIZE_T ssize_t;
//#endif // __SSIZE_T
//
//#else
//#include <sys/select.h>
//#endif

#endif // CC_PLATFORM_WIN

#endif /* __PLATFORMDEFINE_WIN_H__*/
