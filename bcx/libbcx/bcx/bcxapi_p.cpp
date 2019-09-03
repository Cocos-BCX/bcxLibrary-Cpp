//
//  bcxapi_p.cpp
//  bcx
//
//  Copyright © 2019 SDKBox. All rights reserved.
//

#include <vector>
#include <thread>

#include <boost/algorithm/string/predicate.hpp>

#include "graphene/chain/protocol/base.hpp"
#include "./bcx_common.hpp"

#include "./errors.h"

#include "bcxapi_p.hpp"
#include "bcxlog.hpp"
#include "bcxtimer.hpp"

#include "./protocol/full_account.hpp"
#include "./protocol/transaction_confirmation.hpp"
#include "./protocol/account_info_for_reg.h"
#include "./protocol/contract_create_operation.hpp"
#include "./protocol/nh_register_creator_operation.hpp"
#include "./protocol/nh_create_world_view_operation.hpp"

#include "lua_types.hpp"

#include "./boostHttpClient.hpp"




using namespace graphene;
using namespace graphene::chain;
NS_BCX_BEGIN

bcxapi_p::bcxapi_p()
{
    _functionsToPerform.reserve(30);
}

bcxapi_p::~bcxapi_p()
{
}


void bcxapi_p::init(const bcxconfig& cfg, const std::function<void(int)>& cb) {
    _config = cfg;
    if (cfg.wsnode.empty()) {
        bcx::log("ws node url is empty");
        cb(ERROR_UNKNOW);
        return;
    }
    _ws.init(cfg.wsnode, [this, cb]() {
        this->query_login(cb);
    });
    
    TimerMgr::instance().addTimer(60*1000, true, [this](int) {
        query_dynamic_global_property_object();
    });
}

void bcxapi_p::createAccount(const std::string &account, const std::string &pw,
                   const std::function<void(bool, const std::string&)> &callback) {
    ::promise::Defer defer;
    if (isLogin()) {
        defer = createAccountByAccount(account, pw);
    } else {
        defer = createAccountByFaucet(account, pw);
    }
    defer.then([=](bool suc, const std::string& error) {
        callback(suc, error);
    })
    .fail([=](bool suc, const std::string& error) {
        callback(suc, error);
    });
}

void bcxapi_p::login(const std::string &account, const std::string &pwd, const std::function<void(int)>&callback) {
    validate_password(account, pwd)
    .then([=](int loginRst) {
        callback(loginRst);
    })
    .fail([=](int error) {
        callback(error);
    })
    .fail([=] {
        callback(ERROR_UNKNOW);
    });
}

void bcxapi_p::logout() {
    this->_db.savedKeys.clear();
}

bool bcxapi_p::isLogin() {
    if (this->_db.savedKeys.size() > 0) {
        return true;
    }
    
    return false;
}

void bcxapi_p::changePassword(const std::string &account,
                            const std::string &oldpw,
                            const std::string &newpw,
                            const std::function<void(bool)> &callback) {
    std::shared_ptr<graphene::chain::account_update_operation> op =
    std::make_shared<graphene::chain::account_update_operation>();
    validate_password(account, oldpw)
    .then([=](int error) {
        if (error) {
            throw std::string("login failed");
        }
        fc::ecc::private_key ownerPriKey = fc::ecc::private_key::generate_from_seed(fc::sha256::hash(account + "owner" + newpw));
        fc::ecc::public_key ownerPubKey = ownerPriKey.get_public_key();
        fc::ecc::private_key activePriKey = fc::ecc::private_key::generate_from_seed(fc::sha256::hash(account + "active" + newpw));
        fc::ecc::public_key activePubKey = activePriKey.get_public_key();
        fc::ecc::private_key memoPriKey = fc::ecc::private_key::generate_from_seed(fc::sha256::hash(account + "memo" + newpw));
        fc::ecc::public_key memoPubKey = memoPriKey.get_public_key();
        
        op->account = this->_db.current_full_account.account.get_id();
        op->owner = graphene::chain::authority(1, graphene::chain::public_key_type(ownerPubKey), 1);
        op->active = graphene::chain::authority(1, graphene::chain::public_key_type(activePubKey), 1);
        
        return this->get_required_fees({*op}, graphene::chain::asset_id_type());
    })
    .then([=](const std::vector<graphene::chain::asset>& vFee) {
        if (vFee.size() < 1) {
            throw std::string("get request fee result size is 0");
        }
        op->fee = vFee[0];
        return this->send_operation({*op});
    })
    .then([=](fc::variant v) {
        callback(true);
    })
    .fail([=](const std::exception& e) {
        bcx::log("%s", e.what());
        callback(false);
    })
    .fail([=](const std::string& e) {
        bcx::log("%s", e.c_str());
        callback(false);
    })
    .fail([=] {
        callback(false);
    });
}

void bcxapi_p::upgradeAccount(const std::function<void(int)> &callback) {
    if (!isLogin()) {
        callback(ERROR_NOT_LOGIN);
        return;
    }
    std::shared_ptr<graphene::chain::account_upgrade_operation> op =
        std::make_shared<graphene::chain::account_upgrade_operation>();

    op->account_to_upgrade = _db.current_full_account.account.get_id();
    op->upgrade_to_lifetime_member = true;

    get_required_fees({*op}, graphene::chain::asset_id_type())
    .then([=](const std::vector<graphene::chain::asset>& vFee) {
        if (vFee.size() < 1) {
            throw std::string("get request fee result size is 0");
        }
        op->fee = vFee[0];
        return this->send_operation({*op});
    })
    .then([=](fc::variant v) {
        callback(SUCCESS);
    })
    .fail([=](const std::exception& e) {
        bcx::log("%s", e.what());
        callback(ERROR_UNKNOW);
    })
    .fail([=](const std::string& e) {
        bcx::log("%s", e.c_str());
        callback(ERROR_FC_EXCEPTION);
    })
    .fail([=] {
        callback(ERROR_UNKNOW);
    });
}

void bcxapi_p::get_full_accounts(const std::string &nameOrId,
                               const std::function<void(graphene::chain::account_object)> &callback)
{
    bool isSubscribe = false;
    // [[nameOrId], isSubscribe]
    fc::variants mparams; //method params
    fc::variants mparam1{nameOrId};
    mparams.push_back(mparam1);
    mparams.push_back(isSubscribe);
    
    _ws.send_call(2, "get_full_accounts", mparams)
    .then([=](fc::variant var) {
        BCXLOG("get_full_accounts> %s", fc::json::to_string(var).data());
        
        if (var.is_null()) {
            return ;
        }
        
        auto result = var.get_array();
        auto accountVar = result.at(0).get_array();
        auto name = accountVar[0].as_string();
        
        BCXLOG("name=%s", name.data());
        auto v = accountVar[1].get_object()["account"];
        auto account = v.as<graphene::chain::account_object>(BCX_PACK_MAX_DEPTH);
        BCXLOG("account name=%s", account.name.c_str());
        
        if (callback) {
            callback(account);
        }
        // TODO: statistics
        // TODO: balances
    });
}

void bcxapi_p::get_chain_id(const std::function<void(std::string)> &callback)
{
    //    {"method":"call","params":[2,"get_chain_id",[]],"id":5}
    _ws.send_call(this->_db.dbAPIId, "get_chain_id", fc::variants())
    .then([=](fc::variant v) {
        this->_db.chain_id = v.as_string();
        
        if (callback) {
            callback(this->_db.chain_id);
        }
    });
}

void bcxapi_p::get_objects(const std::vector<std::string> &ids, const std::function<void(const fc::variants&)>& cb) {
    fc::variant v;
    fc::to_variant(ids, v, BCX_PACK_MAX_DEPTH);
    
    fc::variants vv;
    vv.push_back(v);
    
    _ws.send_call(_db.dbAPIId, "get_objects", vv).then([this, cb](fc::variant v) {
        auto arr = v.get_array();
        //        fc::variant var;
        //        fc::to_variant(arr, var, BCX_PACK_MAX_DEPTH);
        if (cb) { cb(arr); }
    });
}

void bcxapi_p::get_key_references(const std::function<void(std::string)> &callback)
{
    auto opk = get_current_private_key("owner");
    if (!opk.valid()) {
        if (callback) {
            callback("");
        }
        return ;
    }
    fc::ecc::private_key priKey = *opk;
    fc::ecc::public_key ownerKey = priKey.get_public_key();
    
    auto p2 = graphene::chain::public_key_type(ownerKey);
    auto publicKey = fc::variant(p2, (uint32_t)BCX_PACK_MAX_DEPTH);
    std::string s = fc::json::to_string(publicKey, fc::json::stringify_large_ints_and_doubles, BCX_PACK_MAX_DEPTH);
    BCXLOG("public_key_type:%s", s.data());
    
    fc::variants var;
    var.push_back(publicKey);
    fc::variants var2;
    var2.push_back(var);
    
    // {"method":"call","params":[2,"get_key_references",[["COCOS5QGE1dbuJDdWv8Hen7fHC7SiG4G2HKPxcwoYP78PKzhVCR1Mw4"]]],"id":10}
    _ws.send_call(2, "get_key_references", var2)
    .then([=](fc::variant var) {
        BCXLOG("%s:%s", __FUNCTION__, fc::json::to_string(var).data());
        if (var.is_null()) {
            return ;
        }
        
        auto id = var.get_array()[0].get_array()[0].as_string();
        if (callback) {
            callback(id);
        }
        
    });
}


void bcxapi_p::send(const std::string& data)
{
    _ws.send(data);
}

void bcxapi_p::performFunctionInCocosThread(std::function<void ()> function)
{
    std::lock_guard<std::mutex> lock(_performMutex);
    _functionsToPerform.push_back(std::move(function));
}

void bcxapi_p::poolEvent()
{
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

void bcxapi_p::transfer(const std::string& to,
                        const std::string& assetsymbols, int mount,
                        const std::string& payFeeAsset, const std::string& memo_,
                        const std::function<void(int, const std::string&)>& callback) {
    if (!isLogin()) {
        if (callback) {
            callback(ERROR_NOT_LOGIN , "");
        }
        return;
    }
    
    std::shared_ptr<graphene::chain::transfer_operation> op =
    std::make_shared<graphene::chain::transfer_operation>();

    op->from = _db.current_full_account.account.get_id();

    graphene::chain::memo_data memodata;
    memodata.set_message(fc::ecc::private_key(), fc::ecc::public_key(), memo_);
    op->memo = memodata;

    query_get_full_accounts(to)
    .then([=](const fc::variant& v) {
        std::map<string, bcx::protocol::full_account> rst;
        v.as(rst, BCX_PACK_MAX_DEPTH);
        auto fa = rst[to];
        op->to = fa.account.get_id();
        return this->lookup_asset_symbols({assetsymbols, payFeeAsset});
    })
    .then([=](std::vector<graphene::chain::asset_object>& assets) {
        if (2 != assets.size()) {
            throw std::string("assets size is not match 2");
        }
        graphene::chain::asset_object& asset = assets.at(0);
        op->amount = graphene::chain::asset(mount, asset.get_id());
        asset = assets.at(1);

        return this->get_required_fees({*op}, asset.get_id());
    })
    .then([this, op](const std::vector<graphene::chain::asset>& vFee) {
        if (vFee.size() < 1) {
            throw std::string("get request fee result size is 0");
        }
        op->fee = vFee[0];
        return this->send_operation({*op});
    })
    .then([=](fc::variant v) {
        std::vector<bcx::protocol::transaction_confirmation> tcs;
        v.as(tcs, BCX_PACK_MAX_DEPTH);
        std::string trx_id = (std::string)tcs[0].id;
        // auto trxid = v.get_array()[0].get_object()["id"].as_string();
        callback(SUCCESS, trx_id);
    })
    .fail([=](const fc::exception& e) {
        callback(ERROR_FC_EXCEPTION, e.what());
    })
    .fail([=](const std::exception& e) {
        callback(ERROR_UNKNOW, e.what());
    })
    .fail([=](const std::string& e) {
        callback(ERROR_FC_EXCEPTION, e.data());
    })
    .fail([=] {
        callback(ERROR_UNKNOW, "");
    })
    .fail([](void) {
        std::exception_ptr p = std::current_exception();
        bcx::log("unknow error");
    });
}

void bcxapi_p::get_createAsset_fee(const std::function<void(bool)> &callback) {
    fc::variants params;
    //    params.push_back(account);
    //    params.push_back(pwd);
    
    _ws.send_call(1, "login", params)
    .then([callback](const fc::variant v) {
        bool suc = v.as_bool();
        if (callback) {
            callback(suc);
        }
    });
}

void bcxapi_p::lookup_asset_symbols(const std::vector<std::string>& symbols_or_ids,
                                  const std::function<void(std::vector<graphene::chain::asset_object>)> &callback) {
    fc::variants params;
    fc::variant mparams;
    fc::to_variant(symbols_or_ids, mparams, BCX_PACK_MAX_DEPTH);
    params.push_back(mparams);
    
    _ws.send_call(_db.dbAPIId, "lookup_asset_symbols", params)
    .then([callback](const fc::variant v) {
        auto ret = v.as<std::vector<fc::optional<graphene::chain::asset_object>>>(BCX_PACK_MAX_DEPTH);
        if (callback) {
            std::vector<graphene::chain::asset_object> retv;
            for (auto it : ret) {
                if (it.valid()) {
                    retv.push_back(*it);
                }
            }
            callback(retv);
        }
    });
}

void bcxapi_p::list_assets(const std::string& lower_bound_symbol, int limit,
                 const std::function<void(const std::vector<graphene::chain::asset_object>&, int)> &callback) {
    fc::variants params = {lower_bound_symbol, limit};
    
    _ws.send_call(_db.dbAPIId, "list_assets", params)
    .then([callback](const fc::variant v) {
        auto ret = v.as<std::vector<fc::optional<graphene::chain::asset_object>>>(BCX_PACK_MAX_DEPTH);
        if (callback) {
            std::vector<graphene::chain::asset_object> retv;
            for (auto it : ret) {
                if (it.valid()) {
                    retv.push_back(*it);
                }
            }
            callback(retv, SUCCESS);
        }
    })
    .fail([callback]() {
        callback(std::vector<graphene::chain::asset_object>(), ERROR_UNKNOW);
    });
}

void bcxapi_p::create_asset(const std::string& symbol,
                  long long maxSupply,
                  int precision,
                  float exchangeRate,
                  const std::string& description,
                  const std::function<void(int)> &callback) {
    if (!isLogin()) {
        callback(ERROR_NOT_LOGIN);
        return;
    }
    std::shared_ptr<graphene::chain::asset_create_operation> op =
        std::make_shared<graphene::chain::asset_create_operation>();

    op->issuer = _db.current_full_account.account.get_id();
    op->precision = precision;
    op->symbol = symbol;
    op->common_options.description = description;
    op->common_options.max_market_fee = 0;
    op->common_options.max_supply = maxSupply;
    op->common_options.core_exchange_rate = graphene::chain::price(graphene::chain::asset(1),
                                                                   graphene::chain::asset(exchangeRate, asset_id_type(1)));

    get_required_fees({*op}, graphene::chain::asset_id_type())
    .then([=](const std::vector<graphene::chain::asset>& vFee) {
        if (vFee.size() < 1) {
            throw std::string("get request fee result size is 0");
        }
        op->fee = vFee[0];
        return this->send_operation({*op});
    })
    .then([=](fc::variant v) {
        callback(SUCCESS);
    })
    .fail([=](const fc::exception& e) {
        bcx::log("%s", e.what());
        callback(ERROR_FC_EXCEPTION);
    })
    .fail([=](const std::exception& e) {
        bcx::log("%s", e.what());
        callback(ERROR_UNKNOW);
    })
    .fail([=](const std::string& e) {
        bcx::log("%s", e.c_str());
        callback(ERROR_FC_EXCEPTION);
    })
    .fail([=] {
        callback(ERROR_UNKNOW);
    });
}

void bcxapi_p::update_asset(const std::string& symbol,
                  long long maxSupply,
                  float exchangeRate,
                  const std::string& description,
                  const std::function<void(int)> &callback) {
    if (!isLogin()) {
        callback(ERROR_NOT_LOGIN);
        return;
    }
    std::shared_ptr<graphene::chain::asset_update_operation> op =
    std::make_shared<graphene::chain::asset_update_operation>();

    if (0 != maxSupply) {
        op->new_options.max_supply = maxSupply;
    }
    if (!description.empty()) {
        op->new_options.description = description;
    }
    if (0 != exchangeRate) {
        op->new_options.core_exchange_rate
            = graphene::chain::price(graphene::chain::asset(1),
                                     graphene::chain::asset(exchangeRate, asset_id_type(1)));
    }
    
    lookup_asset_symbols({symbol})
    .then([=](std::vector<graphene::chain::asset_object>& assets) {
        if (1 != assets.size()) {
            throw std::string("assets size is not match 1");
        }
        const graphene::chain::asset_object& asset = assets.at(0);
        op->asset_to_update = asset.get_id();
        op->issuer = asset.issuer;
        
        return this->get_required_fees({*op}, graphene::chain::asset_id_type());
    })
    .then([=](const std::vector<graphene::chain::asset>& vFee) {
        if (vFee.size() < 1) {
            throw std::string("get request fee result size is 0");
        }
        op->fee = vFee[0];
        return this->send_operation({*op});
    })
    .then([=](fc::variant v) {
        callback(SUCCESS);
    })
    .fail([=](const fc::exception& e) {
        bcx::log("%s", e.what());
        callback(ERROR_FC_EXCEPTION);
    })
    .fail([=](const std::exception& e) {
        bcx::log("%s", e.what());
        callback(ERROR_UNKNOW);
    })
    .fail([=](const std::string& e) {
        bcx::log("%s", e.c_str());
        callback(ERROR_FC_EXCEPTION);
    })
    .fail([=] {
        callback(ERROR_UNKNOW);
    });
}

void bcxapi_p::issue_asset(const std::string& account,
                         const int mount,
                         const std::string& symbol,
                         const std::string& memo,
                         const std::function<void(int)> &callback) {
    if (!isLogin()) {
        callback(ERROR_NOT_LOGIN);
        return;
    }
    std::shared_ptr<graphene::chain::asset_issue_operation> op =
    std::make_shared<graphene::chain::asset_issue_operation>();

    graphene::chain::memo_data memodata;
    memodata.set_message(fc::ecc::private_key(), fc::ecc::public_key(), memo);
    op->memo = memodata;
    op->issuer = _db.current_full_account.account.get_id();
    op->asset_to_issue = graphene::chain::asset(mount);

    query_get_account_by_name(account)
    .then([=](const graphene::chain::account_object ao) {
        op->issue_to_account = ao.get_id();

        return lookup_asset_symbols({symbol});
    })
    .then([=](std::vector<graphene::chain::asset_object>& assets) {
        if (1 != assets.size()) {
            throw std::string("assets size is not match 1");
        }
        const graphene::chain::asset_object& asset = assets.at(0);
        op->asset_to_issue.asset_id = asset.get_id();

        return this->get_required_fees({*op}, graphene::chain::asset_id_type());
    })
    .then([=](const std::vector<graphene::chain::asset>& vFee) {
        if (vFee.size() < 1) {
            throw std::string("get request fee result size is 0");
        }
        op->fee = vFee[0];
        return this->send_operation({*op});
    })
    .then([=](fc::variant v) {
        callback(SUCCESS);
    })
    .fail([=](const fc::exception& e) {
        bcx::log("%s", e.what());
        callback(ERROR_FC_EXCEPTION);
    })
    .fail([=](const std::exception& e) {
        bcx::log("%s", e.what());
        callback(ERROR_UNKNOW);
    })
    .fail([=](const std::string& e) {
        bcx::log("%s", e.c_str());
        callback(ERROR_FC_EXCEPTION);
    })
    .fail([=] {
        callback(ERROR_UNKNOW);
    });
    // comm_fail_hander();
}

void bcxapi_p::reserve_asset(const std::string& symbol,
                           const int mount,
                           const std::function<void(int)> &callback) {
    if (!isLogin()) {
        callback(ERROR_NOT_LOGIN);
        return;
    }
    std::shared_ptr<graphene::chain::asset_reserve_operation> op =
    std::make_shared<graphene::chain::asset_reserve_operation>();

    op->payer = _db.current_full_account.account.get_id();
    op->amount_to_reserve = graphene::chain::asset(mount);
    
    lookup_asset_symbols({symbol})
    .then([=](std::vector<graphene::chain::asset_object>& assets) {
        if (1 != assets.size()) {
            throw std::string("assets size is not match 1");
        }
        const graphene::chain::asset_object& asset = assets.at(0);
        op->amount_to_reserve.asset_id = asset.get_id();
        
        return this->get_required_fees({*op}, graphene::chain::asset_id_type());
    })
    .then([=](const std::vector<graphene::chain::asset>& vFee) {
        if (vFee.size() < 1) {
            throw std::string("get request fee result size is 0");
        }
        op->fee = vFee[0];
        return this->send_operation({*op});
    })
    .then([=](fc::variant v) {
        callback(SUCCESS);
    })
    .fail([=](const fc::exception& e) {
        bcx::log("%s", e.what());
        callback(ERROR_FC_EXCEPTION);
    })
    .fail([=](const std::exception& e) {
        bcx::log("%s", e.what());
        callback(ERROR_UNKNOW);
    })
    .fail([=](const std::string& e) {
        bcx::log("%s", e.c_str());
        callback(ERROR_FC_EXCEPTION);
    })
    .fail([=] {
        callback(ERROR_UNKNOW);
    });
    // comm_fail_hander();
}

void bcxapi_p::get_block_header(unsigned int num, const std::function<void(const fc::optional<graphene::chain::block_header>&)> &cb) {
    fc::variants params;
    params.push_back(num);
    
    _ws.send_call(_db.dbAPIId, "get_block_header", params)
    .then([=](fc::variant v) {
        auto rst = v.as<fc::optional<block_header>>(BCX_PACK_MAX_DEPTH);
        if (cb) { cb(rst); }
    }).fail([](const std::exception &e) {
        bcx::log("std exception:%s", e.what());
    }).fail([](const fc::exception &e) {
        bcx::log("fc exception:%s", e.what());
    }).fail([](void) {
        // std::exception_ptr p = std::current_exception();
        bcx::log("unknow error");
    });
}

void bcxapi_p::get_block(unsigned int num, const std::function<void(const fc::optional<graphene::chain::signed_block>&)> &cb) {
    fc::variants params;
    params.push_back(num);
    
    _ws.send_call(_db.dbAPIId, "get_block", params)
    .then([=](fc::variant v) {
        //TODO resutl->block_id is not exist on bts
        if (cb) { cb(v.as<fc::optional<signed_block>>(BCX_PACK_MAX_DEPTH)); }
    }).fail([](const std::exception &e) {
        bcx::log("std exception:%s", e.what());
    }).fail([](const fc::exception &e) {
        bcx::log("fc exception:%s", e.what());
    }).fail([](void) {
        // std::exception_ptr p = std::current_exception();
        bcx::log("unknow error");
    });
}

void bcxapi_p::get_account_history(const std::string& account,
                                 const std::string& stop,
                                 unsigned limit,
                                 const std::string& start,
                                 const std::function<void(const std::vector<graphene::chain::operation_history_object>&)>& cb) {
    fc::variants params;
    params.push_back(account);
    params.push_back(stop);
    params.push_back(limit);
    params.push_back(start);
    
    _ws.send_call(_db.historyAPIId, "get_account_history", params)
    .then([=](fc::variant v) {
        //TODO result->result is not same with bitshare
        auto rst = v.as<std::vector<graphene::chain::operation_history_object>>(BCX_PACK_MAX_DEPTH);
        if (cb) { cb(rst); }
    }).fail([](const std::exception &e) {
        bcx::log("%s", e.what());
    }).fail([](const fc::exception &e) {
        bcx::log("%s", e.what());
    }).fail([](void) {
        bcx::log("void");
    });
    
}

void bcxapi_p::create_contract(const std::string& name, const std::string& contract_source,
                               const std::function<void(int)>& callback) {
    if (!isLogin()) {
        callback(ERROR_NOT_LOGIN);
        return;
    }
    std::shared_ptr<bcx::protocol::contract_create_operation> op =
        std::make_shared<bcx::protocol::contract_create_operation>();

//    auto opk = get_current_private_key("active");
//    if (!opk.valid()) {
//        defer.reject("private key not exist");
//        return defer;
//    }

    op->owner = _db.current_full_account.account.get_id();
    op->name = name;
    op->data = contract_source;
    op->contract_authority = _db.current_full_account.account.owner.get_keys().at(0);
    
    get_required_fees({*op}, graphene::chain::asset_id_type())
    .then([=](const std::vector<graphene::chain::asset>& vFee) {
        if (vFee.size() < 1) {
            throw std::string("get request fee result size is 0");
        }
        op->fee = vFee[0];
        return this->send_operation({*op});
    })
    .then([=](fc::variant v) {
        callback(SUCCESS);
    })
    .fail([=](const std::exception& e) {
        bcx::log("%s", e.what());
        callback(ERROR_UNKNOW);
    })
    .fail([=](const std::string& e) {
        bcx::log("%s", e.c_str());
        callback(ERROR_FC_EXCEPTION);
    })
    .fail([=] {
        callback(ERROR_UNKNOW);
    });
}

void bcxapi_p::query_login(const std::function<void(int)> &callback) {
    fc::variants params;
    params.push_back("");
    params.push_back("");
    
    std::shared_ptr<int> suc = std::make_shared<int>();
    
    _ws.send_call(1, "login", params)
    .then([suc, this](fc::variant v) {
        if (v.as_bool()) {
            *suc = SUCCESS;
        } else {
            *suc = ERROR_UNKNOW;
        }
        bcx::log("login callback = %s", suc ? "true" : "false");
        
        return query_api_id("database");
    })
    .then([this](int64_t i) {
        this->_db.dbAPIId = (int)i;
        bcx::log("defer1 callback");
        
        return query_api_id("network_broadcast");
    })
    .then([this](int64_t i) {
        this->_db.networkAPIId = (int)i;
        bcx::log("defer2 callback");
        
        return query_api_id("history");
    })
    .then([this](int64_t i) {
        this->_db.historyAPIId = (int)i;
        bcx::log("defer3 end");
    }).always([suc,this,callback]() {
        BCXLOG("always suc=%s", suc ? "true" : "false");
        BCXLOG("db=%d,net=%d,history=%d", _db.dbAPIId, _db.networkAPIId, _db.historyAPIId);

        if (callback) {
            callback((*suc));
        }
        this->query_dynamic_global_property_object();
        this->get_chain_id(nullptr);
    });
}

::promise::Defer bcxapi_p::query_api_id(const std::string &name, ::promise::Defer defer) {
    _ws.send_call(1, name, fc::variants())
    .then([=](fc::variant v) {
        int64_t i = v.as_int64();
        defer.resolve(i);
    });
    return defer;
}

::promise::Defer bcxapi_p::get_required_fees(const std::vector<graphene::chain::operation> &ops,
                                           graphene::chain::asset_id_type id,
                                           ::promise::Defer defer) {
    fc::variants params;
    fc::variant vops;
    fc::to_variant(ops, vops, BCX_PACK_MAX_DEPTH);
    params.push_back(vops);
    fc::variant idops;
    fc::to_variant(id, idops);
    params.push_back(idops);
    
    _ws.send_call(_db.dbAPIId, "get_required_fees", params)
    .then([defer](const fc::variant v) {
        // auto rst = v.get_object()["result"].as<std::vector<fc::variant>>(BCX_PACK_MAX_DEPTH);
        auto rst = v.as<std::vector<graphene::chain::asset>>(BCX_PACK_MAX_DEPTH);
        defer.resolve(rst);
    });
    
    return defer;
}

::promise::Defer bcxapi_p::broadcast_transaction_with_callback(const graphene::chain::signed_transaction& trx,
                                                               ::promise::Defer defer) {
    fc::variants params;
    fc::variant vtrx;
    fc::to_variant(trx, vtrx, BCX_PACK_MAX_DEPTH);
    params.push_back(vtrx);
    
    _ws.send_call(_db.networkAPIId, "broadcast_transaction_with_callback", params)
    .then([defer](const fc::variant& v) {
        auto rst = v.as<fc::variant>(BCX_PACK_MAX_DEPTH);
        defer.resolve(v);
    })
    .fail([defer](const fc::rpc::error_object& e) {
        defer.reject(e);
    });

    return defer;
}

void bcxapi_p::query_dynamic_global_property_object() {
    this->get_objects({"2.1.0"}, [this](const fc::variants &v) {
        for (auto it : v) {
            auto id = it["id"].as_string();
            if (0 == id.compare("2.1.0")) {
                it.as(this->_db.dgpo, BCX_PACK_MAX_DEPTH);
            }
        }
    });
}

::promise::Defer bcxapi_p::query_get_full_accounts(const std::string &nameOrId, ::promise::Defer defer) {
    bool isSubscribe = false;
    fc::variants mparams;
    fc::variants mparam1{nameOrId};
    mparams.push_back(mparam1);
    mparams.push_back(isSubscribe);
    
    _ws.send_call(_db.dbAPIId, "get_full_accounts", mparams)
    .then([=](fc::variant var) {
        if (var.is_null()) {
            return ;
        }
        defer.resolve(var);
        
        //        auto result = var.get_object()["result"].get_array();
        //        auto accountVar = result.at(0).get_array();
        //        auto name = accountVar[0].as_string();
        //
        //        auto v = accountVar[1].get_object()["account"];
        //        auto account = v.as<graphene::chain::account_object>(20);
    });
    
    return defer;
}

::promise::Defer bcxapi_p::query_get_account_by_name(const std::string &name, ::promise::Defer defer) {
    fc::variants params = {name};

    _ws.send_call(_db.dbAPIId, "get_account_by_name", params)
    .then([=](fc::variant var) {
        if (var.is_null()) {
            defer.reject(std::string("response is null"));
            return ;
        }
        auto rst = var.as<fc::optional<graphene::chain::account_object> >(BCX_PACK_MAX_DEPTH);
        if (rst.valid()) {
            defer.resolve(*rst);
        } else {
            defer.reject(std::string("response is account object"));
        }
    });

    return defer;
}

::promise::Defer bcxapi_p::query_lookup_account_names(const std::vector<std::string> &names, ::promise::Defer defer) {
    fc::variants params;
    fc::variant params1;
    fc::to_variant(names, params1, BCX_PACK_MAX_DEPTH);
    params.push_back(params1);
    
    _ws.send_call(_db.dbAPIId, "lookup_account_names", params)
    .then([=](fc::variant var) {
        if (var.is_null()) {
            defer.reject(std::string("response is null"));
            return ;
        }
        auto rst = var.as<std::vector<fc::optional<graphene::chain::account_object>>>(BCX_PACK_MAX_DEPTH);
        defer.resolve(rst);
    });
    
    return defer;
}

::promise::Defer bcxapi_p::query_get_key_references(const std::vector<public_key_type> &keys, ::promise::Defer defer) {
    fc::variants params;
    fc::variant params1;
    fc::to_variant(keys, params1, BCX_PACK_MAX_DEPTH);
    params.push_back(params1);
    
    // {"method":"call","params":[2,"get_key_references",[["COCOS5QGE1dbuJDdWv8Hen7fHC7SiG4G2HKPxcwoYP78PKzhVCR1Mw4"]]],"id":10}
    _ws.send_call(2, "get_key_references", params)
    .then([=](fc::variant var) {
        if (var.is_null()) {
            defer.reject("result not exist");
            return ;
        }
        defer.resolve(var);
    });
    return defer;
}

::promise::Defer bcxapi_p::send_operation(const std::vector<graphene::chain::operation>& ops,  ::promise::Defer defer) {
    std::shared_ptr<graphene::chain::signed_transaction> trx =
        std::make_shared<graphene::chain::signed_transaction>();
    trx->operations.insert(std::end(trx->operations), std::begin(ops), std::end(ops));
    trx->ref_block_num = this->_db.dgpo.head_block_number;  // 11084;
    trx->set_reference_block(this->_db.dgpo.head_block_id); // trx.ref_block_prefix = 71151825;
    fc::time_point_sec t = fc::time_point::now();
    t += 10;
    trx->set_expiration(t);

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
    .fail([defer] {
        defer.reject("unknow error");
    });

    return defer;
}

fc::optional<fc::ecc::private_key> bcxapi_p::get_current_private_key(const std::string& role) {
    for (auto &it : this->_db.savedKeys) {
        if (boost::algorithm::ends_with(it.first, role)) {
            return fc::optional<fc::ecc::private_key>(it.second);
        }
    }
    return fc::optional<fc::ecc::private_key>();
}

fc::optional<fc::ecc::private_key> bcxapi_p::get_private_key_by_public(const graphene::chain::public_key_type pkeyt) {
    for (auto &it : this->_db.savedKeys) {
        graphene::chain::public_key_type tpkeyt = it.second.get_public_key();
        if (tpkeyt == pkeyt) {
            return fc::optional<fc::ecc::private_key>(it.second);
        }
    }
    return fc::optional<fc::ecc::private_key>();
}

::promise::Defer bcxapi_p::validate_password(const std::string &account, const std::string &pwd, ::promise::Defer defer) {
    query_get_full_accounts(account)
    .then([=](const fc::variant& v) {
        std::map<string, bcx::protocol::full_account> rst;
        v.as(rst, BCX_PACK_MAX_DEPTH);
        auto fa = rst[account];
        this->_db.current_full_account = fa;
        
        fc::ecc::private_key ownerPriKey = fc::ecc::private_key::generate_from_seed(fc::sha256::hash(account + "owner" + pwd));
        fc::ecc::public_key ownerPubKey = ownerPriKey.get_public_key();
        fc::ecc::private_key activePriKey = fc::ecc::private_key::generate_from_seed(fc::sha256::hash(account + "active" + pwd));
        fc::ecc::public_key activePubKey = activePriKey.get_public_key();
        fc::ecc::private_key memoPriKey = fc::ecc::private_key::generate_from_seed(fc::sha256::hash(account + "memo" + pwd));
        fc::ecc::public_key memoPubKey = memoPriKey.get_public_key();

//        auto opub = graphene::chain::public_key_type(ownerPubKey);
//        auto apub = graphene::chain::public_key_type(activePubKey);
//        auto mpub = graphene::chain::public_key_type(memoPubKey);

//        bcx::log("%s owner %s", account.c_str(), ((std::string)opub).data());
//        bcx::log("%s active %s", account.c_str(), ((std::string)apub).data());
//        bcx::log("%s memo %s", account.c_str(), ((std::string)mpub).data());
        int loginRst = ERROR_UNKNOW;
        for (auto it : fa.account.owner.key_auths) {
            if (it.first == graphene::chain::public_key_type(ownerPubKey)) {
                this->_db.savedKeys[account + "owner"] = ownerPriKey;
                loginRst = SUCCESS;
                break;
            }
        }
        
        for (auto it : fa.account.active.key_auths) {
            if (it.first == graphene::chain::public_key_type(activePubKey)) {
                this->_db.savedKeys[account + "active"] = activePriKey;
                loginRst = SUCCESS;
                break;
            }
        }
        
        if (fa.account.options.memo_key == graphene::chain::public_key_type(memoPubKey)) {
            this->_db.savedKeys[account + "memo"] = memoPriKey;
            loginRst = SUCCESS;
        }
        defer.resolve(loginRst);
    })
    .fail([=](const fc::exception& e) {
        defer.reject(ERROR_UNKNOW);
    });
    
    return defer;
}

::promise::Defer bcxapi_p::createAccountByAccount(const std::string &account, const std::string &pw,
                                                ::promise::Defer defer) {
    std::shared_ptr<graphene::chain::account_create_operation> op =
        std::make_shared<graphene::chain::account_create_operation>();

    fc::ecc::private_key ownerPriKey = fc::ecc::private_key::generate_from_seed(fc::sha256::hash(account + "owner" + pw));
    fc::ecc::public_key ownerPubKey = ownerPriKey.get_public_key();
    fc::ecc::private_key activePriKey = fc::ecc::private_key::generate_from_seed(fc::sha256::hash(account + "active" + pw));
    fc::ecc::public_key activePubKey = activePriKey.get_public_key();
    fc::ecc::private_key memoPriKey = fc::ecc::private_key::generate_from_seed(fc::sha256::hash(account + "memo" + pw));
    fc::ecc::public_key memoPubKey = memoPriKey.get_public_key();

    op->name = account;
    op->registrar = _db.current_full_account.account.get_id();
    op->referrer = _db.current_full_account.account.get_id();
    op->owner = graphene::chain::authority(1, graphene::chain::public_key_type(ownerPubKey), 1);
    op->active = graphene::chain::authority(1, graphene::chain::public_key_type(activePubKey), 1);
    op->options.memo_key = memoPubKey;

    get_required_fees({*op}, graphene::chain::asset_id_type())
    .then([=](const std::vector<graphene::chain::asset>& vFee) {
        if (vFee.size() < 1) {
            throw "get request fee result size is 0";
        }
        op->fee = vFee[0];
        return this->send_operation({*op});
    })
    .then([defer](fc::variant v) {
        defer.resolve(true, "");
    })
    .fail([=](const std::exception& e) {
        defer.reject(false, std::string(e.what()));
    })
    .fail([defer](const std::string& e) {
        defer.reject(false, e);
    })
    .fail([defer](const fc::rpc::error_object& e) {
        defer.reject(false, e.message);
    })
    .fail([defer] {
        defer.reject(false, "unknow error");
    });

    return defer;
}

::promise::Defer bcxapi_p::createAccountByFaucet(const std::string &account, const std::string &pw,
                                               ::promise::Defer defer) {
    bcx::protocol::account_info_for_reg acc;

    fc::ecc::private_key ownerPriKey = fc::ecc::private_key::generate_from_seed(fc::sha256::hash(account + "owner" + pw));
    fc::ecc::public_key ownerPubKey = ownerPriKey.get_public_key();
    fc::ecc::private_key activePriKey = fc::ecc::private_key::generate_from_seed(fc::sha256::hash(account + "active" + pw));
    fc::ecc::public_key activePubKey = activePriKey.get_public_key();
    fc::ecc::private_key memoPriKey = fc::ecc::private_key::generate_from_seed(fc::sha256::hash(account + "memo" + pw));
    fc::ecc::public_key memoPubKey = memoPriKey.get_public_key();

    acc.account.name = account;
    acc.account.owner_key = std::string(graphene::chain::public_key_type(ownerPubKey));
    acc.account.active_key = std::string(graphene::chain::public_key_type(activePubKey));
    acc.account.memo_key = std::string(graphene::chain::public_key_type(memoPubKey));

    std::thread t(bcx::boostHttpClient::httpPost,
                  _config.faucet_url,
                  fc::json::to_string(fc::variant(acc, (uint32_t)BCX_PACK_MAX_DEPTH), fc::json::stringify_large_ints_and_doubles, BCX_PACK_MAX_DEPTH),
                  [this, defer](const std::string& resp, const std::string& error) {
                      this->performFunctionInCocosThread([defer, resp, error]() {
                          if (!error.empty()) {
                              defer.reject(false, error);
                              return ;
                          }
                          fc::variant v = fc::json::from_string(resp);
                          int code = 0;
                          if (v.get_object().contains("code")) {
                              code = (int)v.get_object()["code"].as_int64();
                          }
                          if (200 != code) {
                              //the msg is unicode, ignore it
//                              std::string msg = "";
//                              if (v.get_object().contains("msg")) {
//                                  msg = v.get_object()["msg"].as_string();
//                              }
                              defer.reject(false, std::string(""));
                              return;
                          }
                          defer.resolve(true, error);
                      });
    });
    t.detach();
    
    bcx::log("createAccountByFaucet");

    return defer;
}

void bcxapi_p::get_contract(const std::string& nameOrId,
                            const std::function<void(int error, const bcx::protocol::contract_object&)> &callback) {
    query_get_contract(nameOrId)
    .then([callback](const bcx::protocol::contract_object& co) {
        callback(SUCCESS , co);
    })
    .fail([callback]() {
        callback(ERROR_UNKNOW, bcx::protocol::contract_object());
    });
}

void bcxapi_p::update_contract(const std::string& name_or_id, const std::string& contract_source,
                     const std::function<void(int)>& callback) {
    if (!isLogin()) {
        callback(ERROR_NOT_LOGIN);
        return;
    }

    fc::variants params;
    params.push_back(name_or_id);
    
    std::shared_ptr<bcx::protocol::contract_revise_operation> op =
    std::make_shared<bcx::protocol::contract_revise_operation>();

    op->data = contract_source;
    
    _ws.send_call(_db.dbAPIId, "get_contract", params)
    .then([=](fc::variant var) {
        auto co = var.as<bcx::protocol::contract_object>(BCX_PACK_MAX_DEPTH);
//        graphene::chain::object_id_type idt = co->get_id();
//        std::string ids = (std::string)idt;
//        bcx::log("contract id:%s", ids.data());

        if (co.owner != this->_db.current_full_account.account.get_id()) {
            bcx::log("current user not own this contract");
            throw ERROR_NOT_OWN;
        }
        
        op->contract_id = co.get_id();
        op->reviser = co.owner;

        return this->get_required_fees({*op}, graphene::chain::asset_id_type());
    })
    .then([=](const std::vector<graphene::chain::asset>& vFee) {
        if (vFee.size() < 1) {
            throw std::string("get request fee result size is 0");
        }
        op->fee = vFee[0];
        return this->send_operation({*op});
    })
    .fail([callback](const fc::rpc::error_object& eo) {
        bcx::log("block chain error:%lld,%s", eo.code, eo.message.data());
        callback(ERROR_CHAIN);
    })
    .fail([callback](int e) {
        callback(e);
    })
    .fail([callback]() {
        bcx::log("update contract failed");
        callback(ERROR_UNKNOW);
    });
}

void bcxapi_p::get_transaction_by_id(const std::string& trx_id,
                           const std::function<void(int, const graphene::chain::signed_transaction&)>& callback) {
    fc::variants params;
    params.push_back(trx_id);
    
    // [2,"get_transaction_by_id",["947a291001513a91332b105544acbd2092da308e8a90528b7215349733ea7b66"]]
    _ws.send_call(2, "get_transaction_by_id", params)
    .then([=](fc::variant var) {
        auto strx = var.as<fc::optional<graphene::chain::signed_transaction>>(BCX_PACK_MAX_DEPTH);
        if (!strx.valid()) {
            callback(ERROR_NOT_EXIST, graphene::chain::signed_transaction());
        }
        callback(SUCCESS, *strx);
    })
    .fail([=](const fc::exception& e) {
        callback(ERROR_FC_EXCEPTION, graphene::chain::signed_transaction());
    })
    .fail([=](const std::exception& e) {
        callback(ERROR_UNKNOW, graphene::chain::signed_transaction());
    })
    .fail([=]() {
        callback(ERROR_UNKNOW, graphene::chain::signed_transaction());
    });
}

void bcxapi_p::get_account_contract_data(const std::string& accountId, const std::string& contractId) {
    fc::variants params;
    params.push_back(accountId);
    params.push_back(contractId);
    _ws.send_call(2, "get_account_contract_data", params)
    .then([](fc::variant var) {
        
    });
}

void bcxapi_p::call_contract_function(const std::string& nameOrId,
                            const std::string& functionName,
                            const std::vector<bcx::lua_types>& valueList,
                            int runTime,
                            const std::function<void(int)>& callback) {
    if (!isLogin()) {
        callback(ERROR_NOT_LOGIN);
        return;
    }
    std::shared_ptr<bcx::protocol::contract_call_function_operation> op =
    std::make_shared<bcx::protocol::contract_call_function_operation>();

    op->caller = _db.current_full_account.account.get_id();
    op->function_name = functionName;

    op->value_list = valueList;
    //    std::vector<std::string> value_list;

    fc::variants params;
    params.push_back(nameOrId);
    _ws.send_call(_db.dbAPIId, "get_contract", params)
    .then([=](fc::variant var) {
        auto co = var.as<bcx::protocol::contract_object>(BCX_PACK_MAX_DEPTH);
        op->contract_id = co.get_id();
        
        return this->get_required_fees({*op}, graphene::chain::asset_id_type());
    })
    .then([=](const std::vector<graphene::chain::asset>& vFee) {
        if (vFee.size() < 1) {
            throw std::string("get request fee result size is 0");
        }
        op->fee = vFee[0];
        return this->send_operation({*op});
    })
    .then([=](fc::variant v) {
        callback(SUCCESS);
    })
    .fail([=](const fc::rpc::error_object& eo) {
        bcx::log("%lld, %s", eo.code, eo.message.data());
        callback(ERROR_CHAIN);
    })
    .fail([=](const fc::exception& e) {
        bcx::log("%s", e.what());
        callback(ERROR_UNKNOW);
    })
    .fail([=](const std::exception& e) {
        bcx::log("%s", e.what());
        callback(ERROR_UNKNOW);
    })
    .fail([=](const std::string& e) {
        bcx::log("%s", e.c_str());
        callback(ERROR_FC_EXCEPTION);
    })
    .fail([=] {
        callback(ERROR_UNKNOW);
    });
}

void bcxapi_p::nh_register_creator(const std::function<void(int)> &callback) {
    if (!isLogin()) {
        callback(ERROR_NOT_LOGIN);
        return;
    }
    std::shared_ptr<bcx::protocol::nh_register_creator_operation> op =
        std::make_shared<bcx::protocol::nh_register_creator_operation>();
    
    op->fee_paying_account = _db.current_full_account.account.get_id();

    get_required_fees({*op}, graphene::chain::asset_id_type())
    .then([=](const std::vector<graphene::chain::asset>& vFee) {
        if (vFee.size() < 1) {
            throw std::string("get request fee result size is 0");
        }
        op->fee = vFee[0];
        return this->send_operation({*op});
    })
    .then([=](fc::variant v) {
        callback(SUCCESS);
    })
    .fail([=](const std::exception& e) {
        bcx::log("%s", e.what());
        callback(ERROR_UNKNOW);
    })
    .fail([=](const std::string& e) {
        bcx::log("%s", e.c_str());
        callback(ERROR_FC_EXCEPTION);
    })
    .fail([=] {
        callback(ERROR_UNKNOW);
    });
}

void bcxapi_p::nh_create_world_view(const std::string& name , const std::function<void(int)> &callback) {
    if (!isLogin()) {
        callback(ERROR_NOT_LOGIN);
        return;
    }
    std::shared_ptr<bcx::protocol::nh_create_world_view_operation> op =
        std::make_shared<bcx::protocol::nh_create_world_view_operation>();
    
    op->fee_paying_account = _db.current_full_account.account.get_id();
    op->world_view = name;

    get_required_fees({*op}, graphene::chain::asset_id_type())
    .then([=](const std::vector<graphene::chain::asset>& vFee) {
        if (vFee.size() < 1) {
            throw std::string("get request fee result size is 0");
        }
        op->fee = vFee[0];
        return this->send_operation({*op});
    })
    .then([=](fc::variant v) {
        callback(SUCCESS);
    })
    .fail([=](const fc::rpc::error_object& eo) {
        bcx::log("%lld, %s", eo.code, eo.message.data());
        callback(ERROR_CHAIN);
    })
    .fail([=](const std::exception& e) {
        bcx::log("%s", e.what());
        callback(ERROR_UNKNOW);
    })
    .fail([=](const std::string& e) {
        bcx::log("%s", e.c_str());
        callback(ERROR_FC_EXCEPTION);
    })
    .fail([=] {
        callback(ERROR_UNKNOW);
    });
}

void bcxapi_p::nh_relate_world_view(const std::string& name , const std::function<void(int)> &callback) {
    if (!isLogin()) {
        callback(ERROR_NOT_LOGIN);
        return;
    }
    std::shared_ptr<bcx::protocol::nh_relate_world_view_operation> op =
    std::make_shared<bcx::protocol::nh_relate_world_view_operation>();
    
    op->related_account = _db.current_full_account.account.get_id();
    op->view_owner = _db.current_full_account.account.get_id();
    op->world_view = name;
    
    get_required_fees({*op}, graphene::chain::asset_id_type())
    .then([=](const std::vector<graphene::chain::asset>& vFee) {
        if (vFee.size() < 1) {
            throw std::string("get request fee result size is 0");
        }
        op->fee = vFee[0];
        return this->send_operation({*op});
    })
    .then([=](fc::variant v) {
        callback(SUCCESS);
    })
    .fail([=](const fc::rpc::error_object& eo) {
        bcx::log("%lld, %s", eo.code, eo.message.data());
        callback(ERROR_CHAIN);
    })
    .fail([=](const std::exception& e) {
        bcx::log("%s", e.what());
        callback(ERROR_UNKNOW);
    })
    .fail([=](const std::string& e) {
        bcx::log("%s", e.c_str());
        callback(ERROR_FC_EXCEPTION);
    })
    .fail([=] {
        callback(ERROR_UNKNOW);
    });
}

void bcxapi_p::nh_creat_asset(const std::vector<bcx::nh_asset_create_info>& nh_assets,
                              const std::function<void(int, const std::string&)> &callback) {
    if (!isLogin()) {
        callback(ERROR_NOT_LOGIN, "");
        return;
    }
    std::shared_ptr<std::vector<graphene::chain::operation>> ops =
    std::make_shared<std::vector<graphene::chain::operation>>();
    
    std::shared_ptr<std::vector<std::string>> asset_names =
    std::make_shared<std::vector<std::string>>();
    
    for (const auto& asset_create : nh_assets) {
        bcx::protocol::nh_create_asset_operation op;

        op.fee_paying_account = _db.current_full_account.account.get_id();
        op.asset_id = asset_create.asset_name;
        op.world_view = asset_create.world_view;
        op.base_describe = asset_create.base_describe;
        
        ops->push_back(op);
        
        asset_names->push_back(asset_create.owner);
    }
    
    query_lookup_account_names(*asset_names)
    .then([=](const std::vector<fc::optional<graphene::chain::account_object>>& aos) {
        if (ops->size() != aos.size()) {
            throw ERROR_UNKNOW;
        }
        for (int i = 0; i < ops->size(); i++) {
            const auto &ao = aos[i];
            if (ao.valid()) {
                auto &caop = ops->at(i).get<bcx::protocol::nh_create_asset_operation>();
                caop.owner = ao->get_id();
            } else {
                throw ERROR_UNKNOW;
            }
        }

        return get_required_fees(*ops, graphene::chain::asset_id_type());
    })
    .then([=](const std::vector<graphene::chain::asset>& vFee) {
        if (vFee.size() < 1) {
            throw std::string("get request fee result size is 0");
        }
        if (ops->size() != vFee.size()) {
            throw ERROR_UNKNOW;
        }
        for (int i = 0; i < ops->size(); i++) {
            auto &caop = ops->at(i).get<bcx::protocol::nh_create_asset_operation>();
            caop.fee = vFee[i];
        }
        return this->send_operation(*ops);
    })
    .then([=](fc::variant v) {
        std::vector<bcx::protocol::transaction_confirmation> tcs;
        v.as(tcs, BCX_PACK_MAX_DEPTH);
        const auto oprs = tcs[0].trx.operation_results;
        std::string asset_id = "";
        for (const auto& opr : oprs) {
            if (2 == opr.which()) {
                const auto gr = opr.get<bcx::protocol::operation_result_game_id_type>();
                graphene::chain::object_id_type idtype = gr.result;
                asset_id = (std::string)idtype;
            }
        }
        if (asset_id.empty()) {
            callback(ERROR_UNKNOW, "");
        } else {
            callback(SUCCESS, asset_id);
        }
    })
    .fail([=](const fc::rpc::error_object& eo) {
        bcx::log("%lld, %s", eo.code, eo.message.data());
        callback(ERROR_CHAIN, "");
    })
    .fail([=](const std::exception& e) {
        bcx::log("%s", e.what());
        callback(ERROR_UNKNOW, "");
    })
    .fail([=](const std::string& e) {
        bcx::log("%s", e.c_str());
        callback(ERROR_FC_EXCEPTION, "");
    })
    .fail([=] {
        callback(ERROR_UNKNOW, "");
    });
}

void bcxapi_p::nh_delete_asset(const std::vector<std::string>& ids_or_hashs, const std::function<void(int)> &callback) {
    std::shared_ptr<std::vector<graphene::chain::operation>> ops =
    std::make_shared<std::vector<graphene::chain::operation>>();

    query_nh_lookup_assets(ids_or_hashs)
    .then([=](const std::vector<fc::optional<bcx::nh_asset_info>> &assets) {
        for (const auto& asset : assets) {
            if (!asset.valid()) {
                throw ERROR_UNKNOW;
            }
            graphene::chain::account_id_type ait;
            fc::from_variant(asset->nh_asset_owner, ait);
            if (ait != _db.current_full_account.account.get_id()) {
                throw ERROR_NOT_OWN;
            }
            
            bcx::protocol::nh_delete_asset_operation op;
            
            op.fee_paying_account = _db.current_full_account.account.get_id();
            op.nh_asset = asset->id;

            ops->push_back(op);
        }
        
        return get_required_fees(*ops, graphene::chain::asset_id_type());
    })
    .then([=](const std::vector<graphene::chain::asset>& vFee) {
        if (vFee.size() < 1) {
            throw std::string("get request fee result size is 0");
        }
        if (ops->size() != vFee.size()) {
            throw ERROR_UNKNOW;
        }
        for (int i = 0; i < ops->size(); i++) {
            auto &daop = ops->at(i).get<bcx::protocol::nh_delete_asset_operation>();
            daop.fee = vFee[i];
        }
        return this->send_operation(*ops);
    })
    .then([=](fc::variant v) {
        callback(SUCCESS);
    })
    .fail([=](const fc::rpc::error_object& eo) {
        bcx::log("%lld, %s", eo.code, eo.message.data());
        callback(ERROR_CHAIN);
    })
    .fail([=](const std::exception& e) {
        bcx::log("%s", e.what());
        callback(ERROR_UNKNOW);
    })
    .fail([=](const std::string& e) {
        bcx::log("%s", e.c_str());
        callback(ERROR_FC_EXCEPTION);
    })
    .fail([=] {
        callback(ERROR_UNKNOW);
    });
}

void bcxapi_p::nh_transfer_asset(const std::string id_or_hash,
                       const std::string to_account,
                       const std::function<void(int)> &callback) {
    if (!isLogin()) {
        callback(ERROR_NOT_LOGIN);
        return;
    }
    
    std::shared_ptr<bcx::protocol::nh_transfer_asset_operation> op =
    std::make_shared<bcx::protocol::nh_transfer_asset_operation>();
    op->from = _db.current_full_account.account.get_id();
    
    query_get_account_by_name(to_account)
    .then([=](const graphene::chain::account_object ao) {
        op->to = ao.get_id();
        
        return query_nh_lookup_assets({id_or_hash});
    })
    .then([=](const std::vector<fc::optional<bcx::nh_asset_info>> &assets) {
        if (1 != assets.size()) {
            throw ERROR_UNKNOW;
        }
        for (const auto& asset : assets) {
            if (!asset.valid()) {
                throw ERROR_UNKNOW;
            }
            graphene::chain::account_id_type ait;
            fc::from_variant(asset->nh_asset_owner, ait);
            if (ait != _db.current_full_account.account.get_id()) {
                throw ERROR_NOT_OWN;
            }

            op->nh_asset = asset->id;
        }

        return get_required_fees({*op}, graphene::chain::asset_id_type());
    })
    .then([=](const std::vector<graphene::chain::asset>& vFee) {
        if (1 != vFee.size()) {
            throw std::string("get request fee result size is not 1");
        }
        op->fee = vFee[0];

        return this->send_operation({*op});
    })
    .then([=](fc::variant v) {
        callback(SUCCESS);
    })
    .fail([=](const fc::rpc::error_object& eo) {
        bcx::log("%lld, %s", eo.code, eo.message.data());
        callback(ERROR_CHAIN);
    })
    .fail([=](const std::exception& e) {
        bcx::log("%s", e.what());
        callback(ERROR_UNKNOW);
    })
    .fail([=](const std::string& e) {
        bcx::log("%s", e.c_str());
        callback(ERROR_FC_EXCEPTION);
    })
    .fail([=] {
        callback(ERROR_UNKNOW);
    });
}

void bcxapi_p::nh_lookup_assets(const std::vector<std::string>& ids_or_hashs,
                      const std::function<void(int, const std::vector<fc::optional<bcx::nh_asset_info>>&)> &callback) {
    query_nh_lookup_assets(ids_or_hashs)
    .then([=](const std::vector<fc::optional<bcx::nh_asset_info>> &assets) {
        callback(SUCCESS, assets);
    })
    .fail([=]() {
        callback(ERROR_UNKNOW, std::vector<fc::optional<bcx::nh_asset_info>>());
    });
}

::promise::Defer bcxapi_p::lookup_asset_symbols(const std::vector<std::string>& symbols_or_ids,
                                      ::promise::Defer defer) {
    fc::variants params;
    fc::variant mparams;
    fc::to_variant(symbols_or_ids, mparams, BCX_PACK_MAX_DEPTH);
    params.push_back(mparams);
    
    _ws.send_call(_db.dbAPIId, "lookup_asset_symbols", params)
    .then([defer](const fc::variant v) {
        auto ret = v.as<std::vector<fc::optional<graphene::chain::asset_object>>>(BCX_PACK_MAX_DEPTH);

        std::vector<graphene::chain::asset_object> retv;
        for (auto it : ret) {
            if (it.valid()) {
                retv.push_back(*it);
            }
        }
        defer.resolve(retv);
    })
    .fail([defer]() {
        defer.reject();
    });
    
    return defer;
}

void bcxapi_p::comm_fail_hander(::promise::Defer& defer,
                                const std::function<void(int)> &callback) {
    defer
    .fail([=](const fc::exception& e) {
        bcx::log("%s", e.what());
        callback(ERROR_FC_EXCEPTION);
    })
    .fail([=](const std::exception& e) {
        bcx::log("%s", e.what());
        callback(ERROR_UNKNOW);
    })
    .fail([=](const std::string& e) {
        bcx::log("%s", e.c_str());
        callback(ERROR_FC_EXCEPTION);
    })
    .fail([=] {
        callback(ERROR_UNKNOW);
    });
}

::promise::Defer bcxapi_p::query_get_transaction_by_id(const std::string& trx_id, ::promise::Defer defer) {
    fc::variants params;
    params.push_back(trx_id);

    _ws.send_call(2, "get_transaction_by_id", params)
    .then([=](fc::variant var) {
        auto strx = var.as<fc::optional<graphene::chain::signed_transaction>>(BCX_PACK_MAX_DEPTH);
        if (!strx.valid()) {
            defer.reject(ERROR_NOT_EXIST, graphene::chain::signed_transaction());
            return;
        }
        defer.resolve(SUCCESS, *strx);
    })
    .fail([=](const fc::exception& e) {
        defer.reject(ERROR_FC_EXCEPTION, graphene::chain::signed_transaction());
    })
    .fail([=](const std::exception& e) {
        defer.reject(ERROR_UNKNOW, graphene::chain::signed_transaction());
    })
    .fail([=]() {
        defer.reject(ERROR_UNKNOW, graphene::chain::signed_transaction());
    });
    
    return defer;
}

::promise::Defer bcxapi_p::query_get_contract(const std::string& nameOrId, ::promise::Defer defer) {
    fc::variants params;
    params.push_back(nameOrId);
    
    std::shared_ptr<bcx::protocol::contract_object> co =
    std::make_shared<bcx::protocol::contract_object>();

    _ws.send_call(2, "get_contract", params)
    .then([=](fc::variant var) {
        var.as(*co, BCX_PACK_MAX_DEPTH);
        graphene::chain::object_id_type idt = co->get_id();
        std::string ids = (std::string)idt;
        bcx::log("contract id:%s", ids.data());
        
        std::string s0 = "0000000000000000000000000000000000000000000000000000000000000000";
        if (0 == s0.compare(co->current_version)) {
            //TODO need to getobject 2.11.0
            throw std::string("contract version is 0");
        }
        return query_get_transaction_by_id(co->current_version);
    })
    .then([defer, co](int error, const graphene::chain::signed_transaction& strx) {
        bool bFound = false;
        for (const auto& op : strx.operations) {
            //TODO, should handle mutil contract_create_op
            if (43 == op.which()) {
                const auto& contract_create_op = op.get<bcx::protocol::contract_create_operation>();
                co->lua_code = contract_create_op.data;
                bFound = true;
                break;
            }
        }
        if (!bFound) {
            throw std::string("not find contract in transaction");
        }
        defer.resolve(*co);
    })
    .fail([=] (int error, const graphene::chain::signed_transaction& strx) {
        defer.reject(error);
    })
    .fail([=](const fc::exception& e) {
        bcx::log("%s", e.what());
        defer.reject(ERROR_UNKNOW);
    })
    .fail([=](const std::exception& e) {
        bcx::log("%s", e.what());
        defer.reject(ERROR_UNKNOW);
    })
    .fail([=](const std::string& e) {
        bcx::log("%s", e.c_str());
        defer.reject(ERROR_UNKNOW);
    })
    .fail([=] {
        defer.reject(ERROR_UNKNOW);
    });
    
    return defer;
}

::promise::Defer bcxapi_p::query_nh_lookup_assets(const std::vector<std::string>& ids_or_hashs, ::promise::Defer defer) {
    fc::variants params;
    fc::variant mparams;
    fc::to_variant(ids_or_hashs, mparams, BCX_PACK_MAX_DEPTH);
    params.push_back(mparams);
    
    _ws.send_call(_db.dbAPIId, "lookup_nh_asset", params)
    .then([defer](const fc::variant v) {
        auto ret = v.as<std::vector<fc::optional<bcx::nh_asset_info>>>(BCX_PACK_MAX_DEPTH);
        defer.resolve(ret);
    })
    .fail([defer]() {
        defer.reject();
    });
    
    return defer;
}

::promise::Defer bcxapi_p::query_get_potential_signatures(const signed_transaction& trx, ::promise::Defer defer) {
    fc::variants params;
    fc::variant mparams;
    fc::to_variant(trx, mparams, BCX_PACK_MAX_DEPTH);
    params.push_back(mparams);

    _ws.send_call(_db.dbAPIId, "get_potential_signatures", params)
    .then([defer](const fc::variant v) {
        auto ret = v.as<std::set<graphene::chain::public_key_type>>(BCX_PACK_MAX_DEPTH);
        defer.resolve(ret);
    })
    .fail([defer]() {
        defer.reject();
    });
    
    return defer;
}

NS_BCX_END
