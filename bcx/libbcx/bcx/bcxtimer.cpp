//
//  bcxtimer.cpp
//  bcx
//
//  Copyright © 2019 SDKBox. All rights reserved.
//

#include "bcxtimer.hpp"

#include "platform/PlatformMacros.h"


NS_BCX_BEGIN

Timer::Timer(int timerId, int ms, bool isLoop, const TimerCallback& func)
{
    _timerId = timerId;
    _isLoop = isLoop;
    
    if (isLoop) {
        _startTime = std::chrono::system_clock::from_time_t(0);
    } else {
        _startTime = std::chrono::system_clock::now();
    }
    _duration = ms;
    _function = std::move(func);
}

int Timer::getId() const
{
    return _timerId;
}

bool Timer::process()
{
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - _startTime);
    if (duration.count() < _duration) {
        return false;
    }
    
    _function(_timerId);
    
    if (_isLoop) {
        _startTime = std::chrono::system_clock::now();
        return false;
    } else {
        return true;
    }
}


//

TimerMgr & TimerMgr::instance()
{
    static TimerMgr s_instance;
    return s_instance;
}

TimerMgr::TimerMgr()
{
    _currentTimerId = 0;
}

TimerMgr::~TimerMgr()
{
    
}

int TimerMgr::addTimer(int ms, bool loop, TimerCallback func)
{
    Timer* timer = new Timer(++_currentTimerId, ms, loop, func);
    _allTimers.push_back(timer);
    return _currentTimerId;
}

void TimerMgr::processTimer()
{
    for (int i = 0; i < (int)_allTimers.size(); ++i) {
        Timer* timer = _allTimers[i];
        if (timer && timer->process()) {
            delete timer;
            _allTimers[i] = NULL;
        }
    }
    
    for (auto itr = _allTimers.begin(); itr != _allTimers.end();) {
        if (*itr == NULL) {
            itr = _allTimers.erase(itr);
        } else {
            ++itr;
        }
    }
}

NS_BCX_END
