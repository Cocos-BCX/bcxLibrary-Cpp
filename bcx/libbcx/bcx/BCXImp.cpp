#include <iostream>
#include <sstream>

#include <fc/crypto/sha256.hpp>
#include <fc/crypto/elliptic.hpp>
#include <fc/io/json.hpp>

#include "./BCXImp.hpp"
#include "./Response.hpp"
#include "./Timer.hpp"
#include "../log/BCXLog.hpp"
#include "../utils/utils.hpp"

#include "../protocol/types.hpp"
#include "../protocol/asset_object.hpp"
#include "../protocol/operations/transfer.hpp"
#include "../protocol/transaction.hpp"

#define CHECK_LOGIN \
if (!isLogin() && nullptr != cb) { \
    std::string s = fc::json::to_string(Response::createResponse(Errors::Error_Not_Login)); \
    cb(s); \
    return; \
}

#define HANDLE_DEFER_FAIL \
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

void BCXImp::login(const std::string& account, const std::string& password, const std::function<void(std::string&)>& cb) {
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

void BCXImp::transfer(const std::string& toAccount, const std::string& symbol, int mount, const std::string& memo,
                      const std::function<void(std::string&)>& cb) {
    CHECK_LOGIN
    
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
    .then([this, d](const fc::variant v) {
        d.resolve(Response::createResponse(v));
    })
    HANDLE_DEFER_FAIL
    
    return d;
}

::promise::Defer BCXImp::sendOperation(const std::vector<bcx::protocol::operation>& ops) {
//    promise::Defer d = promise::newPromise();

    std::shared_ptr<bcx::protocol::signed_transaction> trx = std::make_shared<bcx::protocol::signed_transaction>();

    trx->operations.insert(std::end(trx->operations), std::begin(ops), std::end(ops));
    trx->ref_block_num = _chainData.dgpo.head_block_number;
    trx->set_reference_block(_chainData.dgpo.head_block_id);
    fc::time_point_sec t = fc::time_point::now();
    t += 10;
    trx->set_expiration(t);
    
    fc::optional<fc::ecc::private_key> opk = getCurrentPrivateKey("active");
    Response resp = Response::createResponse(Errors::Error_Error, "not find active key");
    RETURN_REJECT_DEFER_IF(!opk.valid(), resp)

    trx->sign(*opk, protocol::chain_id_type(_chainData.chainID));
    
    return broadcastTransactionWithCallback(*trx);

    /*
    query_get_potential_signatures(*trx)
    .then([=](const std::set<graphene::chain::public_key_type> &pkeys) {
        fc::optional<fc::ecc::private_key> opk;
        if (0 == pkeys.size()) {
            opk = this->get_current_private_key("active");
        } else {
            for (const auto& it : pkeys) {
                opk = get_private_key_by_public(it);
                if (opk.valid()) {
                    break;
                }
            }
        }
        if (!opk.valid()) {
            throw std::string("private key not found");
        }
        trx->sign(*opk, chain_id_type(this->_db.chain_id));
        trx->validate();

        return this->broadcast_transaction_with_callback(*trx);
    })
    .then([defer](const fc::variant& v) {
        defer.resolve(v);
    })
    .fail([defer](const fc::rpc::error_object& e) {
        defer.reject(e);
    })
    .fail([=](const std::exception& e) {
        defer.reject(e);
    })
    .fail([defer](const std::string& e) {
        defer.reject(e);
    })
    .fail([defer](const fc::rpc::error_object& e) {
        defer.reject(e);
    })
    .fail([=](int error) {
        defer.reject(error);
    })
    .fail([defer] {
        defer.reject("unknow error");
    });
     */

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

bool BCXImp::isLogin() {
    if (_chainData.savedKeys.size() > 0) {
        return true;
    }
    return false;
}

}