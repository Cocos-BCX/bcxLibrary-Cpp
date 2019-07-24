//
//  bcxtimer.hpp
//  bcx
//
//  Copyright © 2019 SDKBox. All rights reserved.
//

#pragma once

#include "platform/PlatformMacros.h"
#include <functional>
#include <vector>
#include <chrono>

namespace bcx {
    typedef std::function<void(int)> TimerCallback;
    
    class Timer
    {
    public:
        Timer(int timerId, int ms, bool isLoop, const TimerCallback& func);
        virtual ~Timer() {}
        int getId() const;
        bool process();
        
    private:
        int _timerId;
        bool _isLoop;

        std::chrono::system_clock::time_point _startTime;
        unsigned int _duration;
        
        TimerCallback _function;
    };
    
    class TimerMgr
    {
    public:
        static TimerMgr & instance();
    
        int addTimer(int ms, bool loop, TimerCallback func);
        void delTimer(int timerId);
        void processTimer();
        
    private:
        TimerMgr();
        ~TimerMgr();
        
        unsigned int _currentTimerId;
        std::vector<Timer*> _allTimers;
    };
}
