#include <iostream>
#include <sstream>

#include <fc/crypto/sha256.hpp>
#include <fc/crypto/elliptic.hpp>
#include <fc/io/json.hpp>
#include <fc/rpc/state.hpp>

#include "./BCXImp.hpp"
#include "./Response.hpp"
#include "./Timer.hpp"
#include "../log/BCXLog.hpp"
#include "../utils/utils.hpp"

#include "../protocol/types.hpp"
#include "../protocol/asset_object.hpp"
#include "../protocol/operations/transfer.hpp"
#include "../protocol/transaction.hpp"

#define CHECK_LOGIN(CALLBACK) \
if (!isLogin() && nullptr != CALLBACK) { \
    std::string s = fc::json::to_string(Response::createResponse(Errors::Error_Not_Login)); \
    CALLBACK(s); \
    return; \
}

#define HANDLE_DEFER_FAIL \
.fail([d](const fc::rpc::error_object& e) { \
    d.reject(Response::createResponse(e)); \
}) \
.fail([d](const Response& resp) { \
    d.reject(resp); \
}) \
.fail([d] { \
    Response resp = Response::createResponse(Errors::Error_Unknow); \
    d.reject(resp); \
});

#define HANDLE_DEFER_FAIL_AND_CALL_BACK(CALLBACK) \
.fail([CALLBACK](const Response& resp) { \
    std::string s = fc::json::to_string(resp); \
    CALLBACK(s); \
}) \
.fail([CALLBACK](const fc::exception& e) { \
    Response resp = Response::createResponse(Errors::Error_Chain_Exception); \
    resp.msg = e.to_detail_string(); \
    std::string s = fc::json::to_string(resp); \
    CALLBACK(s); \
}) \
.fail([CALLBACK]() { \
    Response resp = Response::createResponse(Errors::Error_Unknow); \
    std::string s = fc::json::to_string(resp); \
    CALLBACK(s); \
});


#define CALLBACK_AND_RETURN_IF_FAIL(CALLBACK) \
if (Errors::Error_Success != resp.code) { \
    std::string s; \
    s = fc::json::to_string(resp); \
    CALLBACK(s); \
    return; \
}

#define RETURN_REJECT_DEFER_IF(SUC, RESP) \
if (SUC) { \
    return promise::newPromise([RESP](promise::Defer d) { \
        TimerMgr::instance().addTimer([d, RESP](int i) { \
            d.reject(RESP); \
        }, 10, false); \
    }); \
}


namespace bcx {

BCXImp *BCXImp::gInstance = nullptr;

BCXImp* BCXImp::getInstance() {
    if (nullptr == gInstance) {
        gInstance = new BCXImp();
    }
    
    return gInstance;
}

BCXImp::BCXImp() {
}

void BCXImp::init(const Config& cfg) {
    std::cout << "BCXImp::init" << std::endl;
    _cfg = cfg;

    resetChainData();

    TimerMgr::instance().addTimer([this](int i) {
        if (!this->isChainAPIOpen()) {
            return;
        }
        this->getObjects({"2.1.0"})
        .then([this](const Response& resp) {
            const fc::variant& v = resp.data;
            auto arr = v.get_array();
            for (auto it : arr) {
                auto id = it["id"].as_string();
                if (0 == id.compare("2.1.0")) {
                    it.as(this->_chainData.dgpo, BCX_PACK_MAX_DEPTH);
                    break;
                }
            }
        });
    }, 1000 * 50, true);
}

void BCXImp::connect(const std::function<void(const std::string&)>& connectStatus) {
    _ws.setAutoConnect(_cfg.autoConnect);
    
    this->_ws.init(_cfg.wsNode, [=](const Response& resp) {
        if (Errors::Error_Net_Connect == resp.code && !this->isChainAPIOpen()) {
            this->enableChainAPI()
            .always([=]() {
                if (nullptr != connectStatus) {
                    std::string s = fc::json::to_string(resp);
                    connectStatus(s);
                }
            });
        } else {
            if (Errors::Error_Net_Disconnect == resp.code) {
                this->resetChainData();
            }
            if (nullptr != connectStatus) {
                std::string s = fc::json::to_string(resp);
                connectStatus(s);
            }
        }
    });
}

void BCXImp::disconnect() {
    resetChainData();
    _ws.close();
}

void BCXImp::login(const std::string& account, const std::string& password, const std::function<void(const std::string&)>& cb) {
    getFullAccounts(account)
    .then([=](const Response& resp) {
        const fc::variant& v = resp.data;
        std::map<std::string, bcx::protocol::full_account> accountMap;
        v.as(accountMap, BCX_PACK_MAX_DEPTH);
        const bcx::protocol::full_account& fa = accountMap[account];

        fc::ecc::private_key ownerPriKey = fc::ecc::private_key::generate_from_seed(fc::sha256::hash(account + "owner" + password));
        fc::ecc::public_key ownerPubKey = ownerPriKey.get_public_key();
        fc::ecc::private_key activePriKey = fc::ecc::private_key::generate_from_seed(fc::sha256::hash(account + "active" + password));
        fc::ecc::public_key activePubKey = activePriKey.get_public_key();
        fc::ecc::private_key memoPriKey = fc::ecc::private_key::generate_from_seed(fc::sha256::hash(account + "memo" + password));
        fc::ecc::public_key memoPubKey = memoPriKey.get_public_key();

        bool loginSuc = false;
        for (auto it : fa.account.owner.key_auths) {
            if (it.first == bcx::protocol::public_key_type(ownerPubKey)) {
                this->_chainData.savedKeys[account + "owner"] = ownerPriKey;
                loginSuc = true;
                break;
            }
        }
        
        for (auto it : fa.account.active.key_auths) {
            if (it.first == bcx::protocol::public_key_type(activePubKey)) {
                this->_chainData.savedKeys[account + "active"] = activePriKey;
                loginSuc = true;
                break;
            }
        }
        
        std::string s = "";
        if (loginSuc) {
            this->_chainData.fullAccount = fa;
            s = fc::json::to_string(Response::createResponse(Errors::Error_Success));
        } else {
            s = fc::json::to_string(Response::createResponse(Errors::Error_Auth_Fail));
        }
        cb(s);
    })
    HANDLE_DEFER_FAIL_AND_CALL_BACK(cb)

}

void BCXImp::logout() {
    _chainData.savedKeys.clear();
}

bool BCXImp::isLogin() {
    return _chainData.savedKeys.size() > 0;
}

void BCXImp::getFullAccount(const std::string& nameOrId, const std::function<void(const std::string&)>& cb) {
    getFullAccounts(nameOrId)
    .then([=](const Response& resp) {
        std::string s = "";
        s = fc::json::to_string(resp);
        cb(s);
    })
    HANDLE_DEFER_FAIL_AND_CALL_BACK(cb)
}

void BCXImp::createAccount(const std::string& name, const std::string& pw, const std::function<void(const std::string&)> &cb) {
    ::promise::Defer defer;
    if (isLogin()) {
        defer = createAccountByAccount(name, pw);
    } else {
        defer = createAccountByFaucet(name, pw);
    }
    defer.then([=](const Response& resp) {
        std::string s = fc::json::to_string(resp);
        cb(s);
    })
    HANDLE_DEFER_FAIL_AND_CALL_BACK(cb)
}

void BCXImp::changePassword(const std::string& account,
                         const std::string& oldPW,
                         const std::string& newPW,
                         const std::function<void(const std::string&)> &cb) {
    std::shared_ptr<bcx::protocol::account_update_operation> op = std::make_shared<bcx::protocol::account_update_operation>();

    getFullAccounts(account)
    .then([=](const Response& resp) {
        const fc::variant& v = resp.data;
        std::map<std::string, bcx::protocol::full_account> accountMap;
        v.as(accountMap, BCX_PACK_MAX_DEPTH);
        const bcx::protocol::full_account& fa = accountMap[account];

        fc::ecc::private_key ownerPriKey = fc::ecc::private_key::generate_from_seed(fc::sha256::hash(account + "owner" + oldPW));
        fc::ecc::public_key ownerPubKey = ownerPriKey.get_public_key();
        fc::ecc::private_key activePriKey = fc::ecc::private_key::generate_from_seed(fc::sha256::hash(account + "active" + oldPW));
        fc::ecc::public_key activePubKey = activePriKey.get_public_key();
        fc::ecc::private_key memoPriKey = fc::ecc::private_key::generate_from_seed(fc::sha256::hash(account + "memo" + oldPW));
        fc::ecc::public_key memoPubKey = memoPriKey.get_public_key();

        bool loginSuc = false;
        for (auto it : fa.account.owner.key_auths) {
            if (it.first == bcx::protocol::public_key_type(ownerPubKey)) {
                loginSuc = true;
                break;
            }
        }
        
        for (auto it : fa.account.active.key_auths) {
            if (it.first == bcx::protocol::public_key_type(activePubKey)) {
                loginSuc = true;
                break;
            }
        }

        Response r = Response::createResponse(Errors::Error_Auth_Fail);
        RETURN_REJECT_DEFER_IF(!loginSuc, r)

        std::string s = "";
        fc::ecc::private_key newOwnerPriKey = fc::ecc::private_key::generate_from_seed(fc::sha256::hash(account + "owner" + newPW));
        fc::ecc::public_key newOwnerPubKey = newOwnerPriKey.get_public_key();
        fc::ecc::private_key newActivePriKey = fc::ecc::private_key::generate_from_seed(fc::sha256::hash(account + "active" + newPW));
        fc::ecc::public_key newActivePubKey = newActivePriKey.get_public_key();

        op->account = fa.account.get_id();
        op->owner = bcx::protocol::authority(1, bcx::protocol::public_key_type(newOwnerPubKey), 1);
        op->active = bcx::protocol::authority(1, bcx::protocol::public_key_type(newActivePubKey), 1);

        return sendOperation({*op}, ownerPriKey);
    })
    .then([=](const Response& resp) {
        std::string s = fc::json::to_string(resp);
        cb(s);
    })
    HANDLE_DEFER_FAIL_AND_CALL_BACK(cb)
}

void BCXImp::upgradeAccount(const std::function<void(const std::string&)>& cb) {
    CHECK_LOGIN(cb)

    std::shared_ptr<bcx::protocol::account_upgrade_operation> op = std::make_shared<bcx::protocol::account_upgrade_operation>();

    op->account_to_upgrade = _chainData.fullAccount.account.get_id();
    op->upgrade_to_lifetime_member = true;

    sendOperation({*op})
    .then([=](const Response& resp) {
        std::string s = fc::json::to_string(resp);
        cb(s);
    })
    HANDLE_DEFER_FAIL_AND_CALL_BACK(cb)
}

void BCXImp::getChainId(const std::function<void(const std::string&)>& cb) {
    Response resp = Response::createResponse(Errors::Error_Success, _chainData.chainID);
    std::string s = fc::json::to_string(resp);
    cb(s);
}

void BCXImp::getObjects(const std::vector<std::string> &ids, const std::function<void(const std::string&)>& cb) {
    this->getObjects(ids)
    .then([=](const Response& resp) {
        std::string s = fc::json::to_string(resp);
        cb(s);
    })
    HANDLE_DEFER_FAIL_AND_CALL_BACK(cb)
}

void BCXImp::getKeyReferences(const std::function<void(const std::string&)>& cb) {
    if (!cb) {
        return;
    }

    auto opk = getCurrentPrivateKey("owner");
    if (!opk.valid()) {
        std::string s = fc::json::to_string(Response::createResponse(Errors::Error_Not_Login));
        cb(s);
        return;
    }

    fc::ecc::private_key priKey = *opk;
    fc::ecc::public_key ownerKey = priKey.get_public_key();
    
    auto p2 = bcx::protocol::public_key_type(ownerKey);
    auto publicKey = fc::variant(p2, (uint32_t)BCX_PACK_MAX_DEPTH);
//    std::string s = fc::json::to_string(publicKey, fc::json::stringify_large_ints_and_doubles, BCX_PACK_MAX_DEPTH);

    fc::variants var;
    var.push_back(publicKey);
    fc::variants var2;
    var2.push_back(var);

    _ws.send_call(2, "get_key_references", var2)
    .then([=](const fc::variant& v) {
        Response resp = Response::createResponse(v);
        std::string s = fc::json::to_string(resp);
        cb(s);
    })
    HANDLE_DEFER_FAIL_AND_CALL_BACK(cb)
}

void BCXImp::transfer(const std::string& toAccount, const std::string& symbol, int mount, const std::string& memo,
                      const std::function<void(const std::string&)>& cb) {
    CHECK_LOGIN(cb)
    
    std::shared_ptr<bcx::protocol::transfer_operation> op = std::make_shared<bcx::protocol::transfer_operation>();
    op->from = _chainData.fullAccount.account.get_id();
    if (memo.length() > 0) {
        bcx::protocol::memo_data memodata;
        memodata.set_message(fc::ecc::private_key(), fc::ecc::public_key(), memo);
        op->memo = memodata;
    }

    getFullAccounts(toAccount)
    .then([=](const Response& resp) {
        RETURN_REJECT_DEFER_IF(Errors::Error_Success != resp.code, resp)

        const fc::variant& v = resp.data;
        std::map<std::string, bcx::protocol::full_account> accountMap;
        v.as(accountMap, BCX_PACK_MAX_DEPTH);
        const bcx::protocol::full_account& fa = accountMap[toAccount];
        Response r = Response::createResponse(Errors::Error_Error, "can't find to account");
        RETURN_REJECT_DEFER_IF(0 == fa.account.get_id().instance.value, r);
        op->to = fa.account.get_id();

        return this->lookupAssetSymbols({symbol});
    })
    .then([=](const Response& resp) {
        RETURN_REJECT_DEFER_IF(Errors::Error_Success != resp.code, resp)
        
        const fc::variant& v = resp.data;
        auto optional_asset_objects = v.as<std::vector<fc::optional<bcx::protocol::asset_object>>>(BCX_PACK_MAX_DEPTH);
        const auto& optional_asset_object = optional_asset_objects[0];

        if (1 != optional_asset_objects.size() || !optional_asset_object.valid()) {
            Response r = Response::createResponse(Errors::Error_Chain_Error, "fetch symbol failed");
            RETURN_REJECT_DEFER_IF(Errors::Error_Success != r.code, r)
        }

        op->amount = bcx::protocol::asset(mount, (*optional_asset_object).get_id());

        return this->sendOperation({*op});
    })
    .then([=](const Response& resp) {
        std::string s = fc::json::to_string(resp);
        cb(s);
    })
    HANDLE_DEFER_FAIL_AND_CALL_BACK(cb)
    
}

void BCXImp::lookupAssetSymbols(const std::vector<std::string>& symbolsOrIds,
                            const std::function<void(const std::string&)>& cb) {
    lookupAssetSymbols(symbolsOrIds)
    .then([=](const Response& resp) {
        std::string s = fc::json::to_string(resp);
        cb(s);
    })
    HANDLE_DEFER_FAIL_AND_CALL_BACK(cb)
}

void BCXImp::listAssets(const std::string& lowerBoundSymbol, int limit,
                    const std::function<void(const std::string&)>& cb) {
    fc::variants params = {lowerBoundSymbol, limit};

    _ws.send_call(_chainData.dbAPIID, "lookup_asset_symbols", params)
    .then([=](const fc::variant& v) {
        Response resp = Response::createResponse(v);
        std::string s = fc::json::to_string(resp);
        cb(s);
    })
    HANDLE_DEFER_FAIL_AND_CALL_BACK(cb)
}

void BCXImp::createAsset(const std::string& symbol,
                    long long maxSupply,
                    int precision,
                    float exchangeRate,
                    const std::string& description,
                    const std::function<void(const std::string&)>& cb) {
    CHECK_LOGIN(cb)

    std::shared_ptr<bcx::protocol::asset_create_operation> op = std::make_shared<bcx::protocol::asset_create_operation>();

    op->issuer = _chainData.fullAccount.account.get_id();
    op->precision = precision;
    op->symbol = symbol;
    op->common_options.description = description;
    op->common_options.max_market_fee = 0;
    op->common_options.max_supply = maxSupply;
    op->common_options.core_exchange_rate = bcx::protocol::price(bcx::protocol::asset(1 * pow(10, precision), bcx::protocol::asset_id_type(1)),
                                                                 bcx::protocol::asset(exchangeRate * 100000));

    sendOperation({*op})
    .then([=](const Response& resp) {
        std::string s = fc::json::to_string(resp);
        cb(s);
    })
    HANDLE_DEFER_FAIL_AND_CALL_BACK(cb)
}

void BCXImp::updateAsset(const std::string& symbol,
                    long long maxSupply,
                    float exchangeRate,
                    const std::string& description,
                    const std::function<void(const std::string&)>& cb) {
    CHECK_LOGIN(cb)

    std::shared_ptr<bcx::protocol::asset_update_operation> op = std::make_shared<bcx::protocol::asset_update_operation>();

    if (0 != maxSupply) {
        op->new_options.max_supply = maxSupply;
    }
    if (!description.empty()) {
        op->new_options.description = description;
    }

    lookupAssetSymbols({symbol})
    .then([=](const Response& resp) {
        RETURN_REJECT_DEFER_IF(Errors::Error_Success != resp.code, resp)
        
        const fc::variant& v = resp.data;
        auto asset_objects = v.as<std::vector<bcx::protocol::asset_object>>(BCX_PACK_MAX_DEPTH);

        Response r = Response::createResponse(Errors::Error_Error);
        RETURN_REJECT_DEFER_IF(1 != asset_objects.size(), r)

        const bcx::protocol::asset_object& asset = asset_objects.at(0);
        op->asset_to_update = asset.get_id();
        op->issuer = asset.issuer;
        if (0 != exchangeRate) {
            op->new_options.core_exchange_rate = bcx::protocol::price(bcx::protocol::asset(1, asset.get_id()),
                                                                      bcx::protocol::asset(exchangeRate, bcx::protocol::asset_id_type()));
        }

        return this->sendOperation({*op});
    })
    .then([=](const Response& resp) {
        std::string s = fc::json::to_string(resp);
        cb(s);
    })
    HANDLE_DEFER_FAIL_AND_CALL_BACK(cb)

}

void BCXImp::issueAsset(const std::string& account,
                    const int mount,
                    const std::string& symbol,
                    const std::string& memo,
                    const std::function<void(const std::string&)>& cb) {
    CHECK_LOGIN(cb)

    std::shared_ptr<bcx::protocol::asset_issue_operation> op = std::make_shared<bcx::protocol::asset_issue_operation>();

    bcx::protocol::memo_data memodata;
    memodata.set_message(fc::ecc::private_key(), fc::ecc::public_key(), memo);
    op->memo = memodata;
    op->issuer = _chainData.fullAccount.account.get_id();
    op->asset_to_issue = bcx::protocol::asset(mount);

    getAccountByName(account)
    .then([=](const Response& resp) {
        RETURN_REJECT_DEFER_IF(Errors::Error_Success != resp.code, resp)

        const fc::variant& v = resp.data;
        auto account_object = v.as<bcx::protocol::account_object>(BCX_PACK_MAX_DEPTH);

        op->issue_to_account = account_object.get_id();

        return lookupAssetSymbols({symbol});
    })
    .then([=](const Response& resp) {
        RETURN_REJECT_DEFER_IF(Errors::Error_Success != resp.code, resp)

        const fc::variant& v = resp.data;
        auto asset_objects = v.as<std::vector<bcx::protocol::asset_object>>(BCX_PACK_MAX_DEPTH);

        Response r = Response::createResponse(Errors::Error_Error);
        RETURN_REJECT_DEFER_IF(1 != asset_objects.size(), r)

        const bcx::protocol::asset_object& asset = asset_objects.at(0);
        op->asset_to_issue.asset_id = asset.get_id();

        return this->sendOperation({*op});
    })
    .then([=](const Response& resp) {
        std::string s = fc::json::to_string(resp);
        cb(s);
    })
    HANDLE_DEFER_FAIL_AND_CALL_BACK(cb)
}

void BCXImp::reserveAsset(const std::string& symbol,
                    const int mount,
                    const std::function<void(const std::string&)>& cb) {
    CHECK_LOGIN(cb)

    std::shared_ptr<bcx::protocol::asset_reserve_operation> op = std::make_shared<bcx::protocol::asset_reserve_operation>();

    op->payer = _chainData.fullAccount.account.get_id();
    op->amount_to_reserve = bcx::protocol::asset(mount);

    lookupAssetSymbols({symbol})
    .then([=](const Response& resp) {
        RETURN_REJECT_DEFER_IF(Errors::Error_Success != resp.code, resp)

        const fc::variant& v = resp.data;
        auto asset_objects = v.as<std::vector<bcx::protocol::asset_object>>(BCX_PACK_MAX_DEPTH);

        Response r = Response::createResponse(Errors::Error_Error);
        RETURN_REJECT_DEFER_IF(1 != asset_objects.size(), r)

        const bcx::protocol::asset_object& asset = asset_objects.at(0);
        op->amount_to_reserve.asset_id = asset.get_id();

        return this->sendOperation({*op});
    })
    .then([=](const Response& resp) {
        std::string s = fc::json::to_string(resp);
        cb(s);
    })
    HANDLE_DEFER_FAIL_AND_CALL_BACK(cb)
}

void BCXImp::getBlockHeader(unsigned int num, const std::function<void(const std::string&)>& cb) {
    fc::variants params;
    params.push_back(num);
    
    _ws.send_call(_chainData.dbAPIID, "get_block_header", params)
    .then([=](const fc::variant& v) {
        Response resp = Response::createResponse(v);
        std::string s = fc::json::to_string(resp);
        cb(s);
    })
    HANDLE_DEFER_FAIL_AND_CALL_BACK(cb)
}

void BCXImp::getBlock(unsigned int num, const std::function<void(const std::string&)>& cb) {
    fc::variants params;
    params.push_back(num);

    _ws.send_call(_chainData.dbAPIID, "get_block", params)
    .then([=](const fc::variant& v) {
        Response resp = Response::createResponse(v);
        std::string s = fc::json::to_string(resp);
        cb(s);
    })
    HANDLE_DEFER_FAIL_AND_CALL_BACK(cb)
}

void BCXImp::performFunctionInMainThread(const std::function<void()>& f) {
    _looper.performFunctionInMainThread(f);
}

void BCXImp::loop() {
    _looper.loop();
}

::promise::Defer BCXImp::enableChainAPI() {
    promise::Defer d = promise::newPromise();

    fc::variants params;
    params.push_back("");
    params.push_back("");

    _ws.send_call(1, "login", params)
    .then([this](const fc::variant& v) {
        if (v.as_bool()) {
            return this->_ws.send_call(1, "database", fc::variants());
        } else {
            return promise::newPromise([](promise::Defer d) {
                TimerMgr::instance().addTimer([d](int i) {
                    Response resp = Response::createResponse(Errors::Error_Chain_Error);
                    d.reject(resp);
                }, 10, false);
            });
        }
    })
    .then([this](const fc::variant& v) {
        int64_t i = v.as_int64();
        this->_chainData.dbAPIID = (int)i;
        return this->_ws.send_call(1, "network_broadcast", fc::variants());
    })
    .then([this](const fc::variant& v) {
        int64_t i = v.as_int64();
        this->_chainData.networkAPIID = (int)i;
        return this->_ws.send_call(1, "history", fc::variants());
    })
    .then([this](const fc::variant& v) {
        int64_t i = v.as_int64();
        this->_chainData.historyAPIID = (int)i;
        return this->_ws.send_call(this->_chainData.dbAPIID, "get_chain_id", fc::variants());
    })
    .then([this](const fc::variant& v) {
        this->_chainData.chainID = v.as_string();
        return this->getObjects({"2.1.0"});
    })
    .then([this, d](const Response& resp) {
        const fc::variant& v = resp.data;
        auto arr = v.get_array();
        for (auto it : arr) {
            auto id = it["id"].as_string();
            if (0 == id.compare("2.1.0")) {
                it.as(this->_chainData.dgpo, BCX_PACK_MAX_DEPTH);
                break;
            }
        }
        d.resolve(Response::createResponse(Errors::Error_Success));
    })
    .fail([d](const Response& resp) {
        d.reject(resp);
    });

    return d;
}

::promise::Defer BCXImp::getFullAccounts(const std::string &nameOrId) {
    promise::Defer d = promise::newPromise();
    if (!this->isChainAPIOpen()) {
        TimerMgr::instance().addTimer([d](int i) {
            Response resp = Response::createResponse(Errors::Error_API_Disable);
            d.reject(resp);
        }, 10);
        return d;
    }

    bool isSubscribe = false;
    fc::variants mparams;
    fc::variants mparam1{nameOrId};
    mparams.push_back(mparam1);
    mparams.push_back(isSubscribe);

    _ws.send_call(_chainData.dbAPIID, "get_full_accounts", mparams)
    .then([d](const fc::variant& var) {
        d.resolve(Response::createResponse(var));
    })
    .fail([d](const Response& resp) {
        d.reject(resp);
    });

    return d;
}

::promise::Defer BCXImp::getObjects(const std::vector<std::string> &ids) {
    promise::Defer d = promise::newPromise();

    fc::variant v;
    fc::to_variant(ids, v, BCX_PACK_MAX_DEPTH);
    fc::variants vv;
    vv.push_back(v);

    _ws.send_call(this->_chainData.dbAPIID, "get_objects", vv)
    .then([this, d](const fc::variant& v) {
        d.resolve(Response::createResponse(v));
    })
    HANDLE_DEFER_FAIL

    return d;
}

::promise::Defer BCXImp::lookupAssetSymbols(const std::vector<std::string>& symbols_or_ids) {
    promise::Defer d = promise::newPromise();

    fc::variants params;
    fc::variant mparams;
    fc::to_variant(symbols_or_ids, mparams, BCX_PACK_MAX_DEPTH);
    params.push_back(mparams);

    _ws.send_call(_chainData.dbAPIID, "lookup_asset_symbols", params)
    .then([d](const fc::variant v) {
        d.resolve(Response::createResponse(v));
    })
    HANDLE_DEFER_FAIL
    
    return d;
}

::promise::Defer BCXImp::sendOperation(const std::vector<bcx::protocol::operation>& ops, const fc::optional<fc::ecc::private_key>& oSignKey) {
//    promise::Defer d = promise::newPromise();

    std::shared_ptr<bcx::protocol::signed_transaction> trx = std::make_shared<bcx::protocol::signed_transaction>();

    trx->operations.insert(std::end(trx->operations), std::begin(ops), std::end(ops));
    trx->ref_block_num = _chainData.dgpo.head_block_number;
    trx->set_reference_block(_chainData.dgpo.head_block_id);
    fc::time_point_sec t = fc::time_point::now();
    t += 10;
    trx->set_expiration(t);

    fc::optional<fc::ecc::private_key> opk = oSignKey;
    if (!opk.valid()) {
        opk = getCurrentPrivateKey("active");
    }
    Response resp = Response::createResponse(Errors::Error_Error, "not find active key");
    RETURN_REJECT_DEFER_IF(!opk.valid(), resp)

    trx->sign(*opk, protocol::chain_id_type(_chainData.chainID));
    
    return broadcastTransactionWithCallback(*trx);
}

::promise::Defer BCXImp::broadcastTransactionWithCallback(const bcx::protocol::signed_transaction& trx) {
    promise::Defer d = promise::newPromise();
    fc::variants params;
    fc::variant vtrx;
    fc::to_variant(trx, vtrx, BCX_PACK_MAX_DEPTH);
    params.push_back(vtrx);
    
    _ws.send_call(_chainData.networkAPIID, "broadcast_transaction_with_callback", params)
    .then([d](const fc::variant& v) {
        d.resolve(Response::createResponse(v));
    })
    HANDLE_DEFER_FAIL

    return d;
}

::promise::Defer BCXImp::createAccountByAccount(const std::string &account, const std::string &pw) {
    promise::Defer d = promise::newPromise();

    bcx::protocol::account_create_operation op = bcx::protocol::account_create_operation();

    fc::ecc::private_key ownerPriKey = fc::ecc::private_key::generate_from_seed(fc::sha256::hash(account + "owner" + pw));
    fc::ecc::public_key ownerPubKey = ownerPriKey.get_public_key();
    fc::ecc::private_key activePriKey = fc::ecc::private_key::generate_from_seed(fc::sha256::hash(account + "active" + pw));
    fc::ecc::public_key activePubKey = activePriKey.get_public_key();
    fc::ecc::private_key memoPriKey = fc::ecc::private_key::generate_from_seed(fc::sha256::hash(account + "memo" + pw));
    fc::ecc::public_key memoPubKey = memoPriKey.get_public_key();

    op.name = account;
    op.registrar = _chainData.fullAccount.account.get_id();
    op.owner = bcx::protocol::authority(1, bcx::protocol::public_key_type(ownerPubKey), 1);
    op.active = bcx::protocol::authority(1, bcx::protocol::public_key_type(activePubKey), 1);
    op.options.memo_key = memoPubKey;

    sendOperation({op})
    .then([=](const Response& resp) {
        d.resolve(resp);
    })
    HANDLE_DEFER_FAIL

    return d;
}

::promise::Defer BCXImp::createAccountByFaucet(const std::string &account, const std::string &pw) {
    promise::Defer d = promise::newPromise();

    //TODO create account with faucet need implement
    throw fc::invalid_arg_exception();

    return d;
}

::promise::Defer BCXImp::getAccountByName(const std::string &name) {
    promise::Defer d = promise::newPromise();

    fc::variants params = {name};

    _ws.send_call(_chainData.dbAPIID, "get_account_by_name", params)
    .then([this, d](const fc::variant v) {
        d.resolve(Response::createResponse(v));
    })
    HANDLE_DEFER_FAIL

    return d;
}

fc::optional<fc::ecc::private_key> BCXImp::getCurrentPrivateKey(const std::string& role) {
    for (auto &it : this->_chainData.savedKeys) {
        if (Utils::endWith(it.first, role)) {
            return fc::optional<fc::ecc::private_key>(it.second);
        }
    }
    return fc::optional<fc::ecc::private_key>();
}

void BCXImp::resetChainData() {
    _chainData.dbAPIID = -1;
    _chainData.networkAPIID = -1;
    _chainData.historyAPIID = -1;
    _chainData.chainID = "";
}

bool BCXImp::isChainAPIOpen() {
    if (_chainData.historyAPIID >= 0) {
        return true;
    } else {
        return false;
    }
}


}
