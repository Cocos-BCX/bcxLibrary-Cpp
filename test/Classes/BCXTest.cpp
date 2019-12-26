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
        }},
        {"2.1.lookup symbols", [this]() {
            bcx::BCX::lookupAssetSymbols({"COCOS"}, [this](const std::string& json) {
                this->log(json);
            });
        }},
        {"2.2.create asset", [this]() {
            bcx::BCX::createAsset("SDKBOX", 1000, 5, 10, "test asset", [this](const std::string& json) {
                this->log(json);
            });
        }},
        {"2.3.update asset", [this]() {
            bcx::BCX::updateAsset("SDKBOX", 1000, 10, "update test asset", [this](const std::string& json) {
                this->log(json);
            });
        }},
        {"2.4.issue asset", [this]() {
            bcx::BCX::issueAsset("hugo0001", 3, "SDKBOX", "", [this](const std::string& json) {
                this->log(json);
            });
        }},
        {"2.5.reserve asset", [this]() {
            bcx::BCX::reserveAsset("SDKBOX", 3, [this](const std::string& json) {
                this->log(json);
            });
        }},
        {"3.0.get block header", [this]() {
            bcx::BCX::getBlockHeader(62260, [this](const std::string& json) {
                this->log(json);
            });
        }},
        {"3.1.get block", [this]() {
            bcx::BCX::getBlock(62260, [this](const std::string& json) {
                this->log(json);
            });
        }},
        {"4.0.create contract", [this]() {
            std::string contractSource = "\
function set_public_data(key, val) \
    public_data[key] = val \
\
    write_list={public_data={[key]=true}} \
    chainhelper:write_chain() \
end \
\
function get_public_data(key) \
    read_list={public_data={[key]=true}} \
    chainhelper:read_chain() \
\
    chainhelper:log(public_data[key]) \
end \
\
function set_private_data(key, val) \
    private_data[key] = val \
\
    write_list={private_data={[key]=true}} \
    chainhelper:write_chain() \
end \
\
function get_private_data(key) \
    read_list={private_data={[key]=true}} \
    chainhelper:read_chain() \
\
    chainhelper:log(private_data[key]) \
end \
\
function dump_params(param1, param2) \
    chainhelper:log('param1:' .. cjson.encode(param1) .. ' param2:' .. param2) \
end \
\
function set_permissions_flag(flag) \
    assert(is_owner(), \"You are not owner\") \
\
    log('set permissions finish'); \
end";
            bcx::BCX::createContract("contract.test01", contractSource, [this](const std::string& json) {
                this->log(json);
            });
        }},
        {"4.1.get contract", [this]() {
            bcx::BCX::getContract("contract.test01", [this](const std::string& json) {
                this->log(json);
            });
        }},
        {"4.2.update contract", [this]() {
            std::string contractSource = "\
function set_public_data(key, val) \
    public_data[key] = val \
\
    write_list={public_data={[key]=true}} \
    chainhelper:write_chain() \
end \
\
function get_public_data(key) \
    read_list={public_data={[key]=true}} \
    chainhelper:read_chain() \
\
    chainhelper:log(public_data[key]) \
end \
\
function set_private_data(key, val) \
    private_data[key] = val \
\
    write_list={private_data={[key]=true}} \
    chainhelper:write_chain() \
end \
\
function get_private_data(key) \
    read_list={private_data={[key]=true}} \
    chainhelper:read_chain() \
\
    chainhelper:log(private_data[key]) \
end \
\
function dump_params(param1, param2) \
    chainhelper:log('param1:' .. cjson.encode(param1) .. ' param2:' .. param2) \
end \
\
function set_permissions_flag(flag) \
    assert(is_owner(), \"You are not owner\") \
\
    log('set permissions finish'); \
end";

            bcx::BCX::updateContract("contract.test01", contractSource, [this](const std::string& json) {
                this->log(json);
            });
        }},
        {"4.3.call contract", [this]() {
            std::string funName = "dump_params";
            bcx::contract_params params;

            //param1
            bcx::lua_table tab;
            tab[bcx::lua_string("string")] = bcx::lua_value(bcx::lua_string("3331"));
            tab[bcx::lua_string("int")] = bcx::lua_value(bcx::lua_int(2));
            tab[bcx::lua_string("bool")] = bcx::lua_value(bcx::lua_bool(true));
            tab[bcx::lua_string("number")] = bcx::lua_value(bcx::lua_number(3.1415));
            bcx::lua_value v = tab;
            params.push_back(bcx::lua_value(tab));

            //param2
            params.push_back(bcx::lua_string("sdkbox1"));

//            funName = "set_public_data";
//            params.push_back(bcx::lua_string("k1"));
//            params.push_back(bcx::lua_string("v1"));

            bcx::BCX::callContractFunction("contract.test01", funName, params, 5, [this](const std::string& json) {
                this->log(json);
            });
        }}
    };

    _testCaseNames.reserve(_testMap.size());
    for(const auto& kv : _testMap) {
        _testCaseNames.push_back(kv.first);
    }
}
