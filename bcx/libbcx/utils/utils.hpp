#pragma once

#include <string>

class Utils {
public:

    static bool endWith(const std::string &str, const std::string &tail) {
        return str.compare(str.size() - tail.size(), tail.size(), tail) == 0;
    }
     
    static bool startWith(const std::string &str, const std::string &head) {
        return str.compare(0, head.size(), head) == 0;
    }
    
};

