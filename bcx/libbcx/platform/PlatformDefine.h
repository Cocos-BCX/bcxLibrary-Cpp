#pragma once

#include "./PlatformConfig.h"

#if BCX_TARGET_PLATFORM == BCX_PLATFORM_IOS
#include "./ios/PlatformDefine-ios.h"
#elif BCX_TARGET_PLATFORM == BCX_PLATFORM_MAC
#include "./mac/PlatformDefine-mac.h"
#elif BCX_TARGET_PLATFORM == BCX_PLATFORM_ANDROID
#include "./android/PlatformDefine-android.h"
#elif BCX_TARGET_PLATFORM == BCX_PLATFORM_WIN32
#include "./win/PlatformDefine-win.h"
#endif
