#include <sstream>
#include "BCXTest.hpp"
#include "BCX.hpp"

// public
BCXTest::BCXTest() : emptyString("") {
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

int BCXTest::getTestCasesCount() {
    return (int)_testCaseNames.size();
}

const std::string& BCXTest::getTestCasesName(int idx) {
    if (idx >= 0 && idx < _testCaseNames.size()) {
        return _testCaseNames[idx];
    }

    return emptyString;
}

const std::vector<std::string> BCXTest::getTestCases() {
    return _testCaseNames;
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
            bcx::BCX::login("hugo1111", "111111", [this](const std::string& json) {
                this->log(json);
            });
        }},
        {"1.1.create account", [this]() {
            bcx::BCX::createAccount("huang", "111111", [this](const std::string& json) {
                this->log(json);
            });
        }},
        {"1.2.change password", [this]() {
            bcx::BCX::changePassword("hugo1111", "222222", "111111", [this](const std::string& json) {
                this->log(json);
            });
        }},
        {"1.3.upgrade account", [this]() {
            bcx::BCX::upgradeAccount([this](const std::string& json) {
                this->log(json);
            });
        }},
        {"2.0.transfer", [this]() {
            bcx::BCX::transfer("huang", "COCOS", 33, "", [this](const std::string& json) {
                this->log(json);
            });
        }}
    };

    _testCaseNames.reserve(_testMap.size());
    for(const auto& kv : _testMap) {
        _testCaseNames.push_back(kv.first);
    }
}
