#pragma once

#include "../platform/PlatformMacros.h"
#include <functional>
#include <vector>
#include <chrono>
#include <mutex>

namespace bcx {
 
class Looper {

public:
    void performFunctionInMainThread(std::function<void()> f);
    void loop();

private:
    std::vector<std::function<void()>> _functionsToPerform;
    std::mutex _performMutex;

};

}

