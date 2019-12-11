#pragma once

#include <string>
#include <vector>
#include <map>

class BCXTest {
public:
    BCXTest();
    void init();
    const std::vector<std::string> getTestCases();
    void runTestCase(const std::string& funName);
    void loop();
    void setNativeLog(const std::function<void(const std::string&)>& logger);

private:
    void log(const std::string& s);
    void createTestCasesMap();

private:
    std::function<void(const std::string& s)> _nativeLog;
    std::map<std::string, std::function<void(void)> > _testMap;

};
