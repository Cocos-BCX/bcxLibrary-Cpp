#include "Timer.hpp"

namespace bcx {

Timer::Timer(const TimerCallback& func, int timerId, int interMS, bool isLoop) {
    _timerId = timerId;
    _isLoop = isLoop;
    
    if (isLoop) {
        _startTime = std::chrono::system_clock::from_time_t(0);
    } else {
        _startTime = std::chrono::system_clock::now();
    }
    _duration = interMS;
    _function = std::move(func);
}

int Timer::getId() const {
    return _timerId;
}

bool Timer::process() {
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



TimerMgr & TimerMgr::instance() {
    static TimerMgr s_instance;
    return s_instance;
}

TimerMgr::TimerMgr() {
    _currentTimerId = 0;
}

TimerMgr::~TimerMgr() {
}

int TimerMgr::addTimer(TimerCallback func, int interValMS, bool loop) {
    Timer* timer = new Timer(func, ++_currentTimerId, interValMS, loop);
    _allTimers.push_back(timer);
    return _currentTimerId;
}

void TimerMgr::delTimer(int timerId) {
    for (auto it = _allTimers.begin(); it != _allTimers.end(); it++) {
        if (timerId == (*it)->getId()) {
            _allTimers.erase(it);
            break;
        }
    }
}

void TimerMgr::processTimer() {
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

}

