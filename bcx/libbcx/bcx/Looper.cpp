#include "Looper.hpp"
#include "./Timer.hpp"

namespace bcx {

void Looper::performFunctionInMainThread(std::function<void ()> function) {
    std::lock_guard<std::mutex> lock(_performMutex);
    _functionsToPerform.push_back(std::move(function));
}

void Looper::loop() {
    TimerMgr::instance().processTimer();

    if( !_functionsToPerform.empty() ) {
        _performMutex.lock();
        // fixed #4123: Save the callback functions, they must be invoked after '_performMutex.unlock()', otherwise if new functions are added in callback, it will cause thread deadlock.
        auto temp = std::move(_functionsToPerform);
        _performMutex.unlock();
        
        for (const auto &function : temp) {
            function();
        }
    }
}

}

