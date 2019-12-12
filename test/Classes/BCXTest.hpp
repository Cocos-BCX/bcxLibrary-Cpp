#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>

class BCXTest {
public:
    BCXTest();
    void init();
    int getTestCasesCount();
    const std::string& getTestCasesName(int idx);
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
    std::vector<std::string> _testCaseNames;
};
