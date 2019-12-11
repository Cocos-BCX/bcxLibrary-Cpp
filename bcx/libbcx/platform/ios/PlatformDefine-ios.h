#pragma once

#include "../PlatformConfig.h"
#if BCX_TARGET_PLATFORM == BCX_PLATFORM_IOS

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


#endif // CC_PLATFORM_IOS

