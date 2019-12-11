#include <sstream>
#include "BCXTest.hpp"
#include "BCX.hpp"

// public
BCXTest::BCXTest() {
}

void BCXTest::init() {
    createTestCasesMap();

    bcx::Config cfg;
    cfg.autoConnect = true;
    cfg.wsNode = "ws://test.cocosbcx.net";
    cfg.faucetURL = "http://test-faucet.cocosbcx.net";
    bcx::BCX::init(cfg);
    bcx::BCX::connect([this](const std::string& s) {
        std::ostringstream os;
        os << "Connect Status::" << s;
        this->log(os.str());
    });
}

const std::vector<std::string> BCXTest::getTestCases() {
    std::vector<std::string> tests;
    tests.reserve(_testMap.size());
    for(const auto& kv : _testMap) {
        tests.push_back(kv.first);
    }

    return tests;
}

void BCXTest::runTestCase(const std::string& funName) {
    auto it = _testMap.find(funName);
    if (it == _testMap.end()) return ;
    if (it->second == nullptr) return ;

    it->second();
}

void BCXTest::loop() {
    bcx::BCX::loop();
}

void BCXTest::setNativeLog(const std::function<void(const std::string&)>& logger) {
    _nativeLog = logger;
}

void BCXTest::log(const std::string& s) {
    if (nullptr == _nativeLog) {
        return;
    }
    _nativeLog(s);
}

void BCXTest::createTestCasesMap() {
    _testMap = {
        {"1.0.login", [this]() {
            bcx::BCX::login("hugo1111", "111111", [this](std::string& json) {
                this->log(json);
            });
        }},
        {"2.0.transfer", [this]() {
            bcx::BCX::transfer("huang", "COCOS", 33, "", [this](const std::string& json) {
                this->log(json);
            });
        }}
    };
}
