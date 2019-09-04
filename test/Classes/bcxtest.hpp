#pragma once

#include "bcx.hpp"

#include "fc/crypto/elliptic.hpp"
#include "fc/rpc/state.hpp"
#include "fc/io/json.hpp"
#include "fc/rpc/state.hpp"
#include "fc/reflect/variant.hpp"
#include <fc/smart_ref_fwd.hpp>
#include <fc/smart_ref_impl.hpp>
#include <fc/crypto/base58.hpp>

#include <vector>

#include "graphene/chain/protocol/transaction.hpp"
#include "graphene/chain/protocol/asset.hpp"
#include "graphene/chain/protocol/types.hpp"
#include "graphene/chain/protocol/base.hpp"
#include <graphene/chain/protocol/fee_schedule.hpp>
#include <graphene/chain/account_object.hpp>
#include <graphene/chain/balance_object.hpp>
#include <graphene/chain/global_property_object.hpp>

struct BCXTest
{

    void onBtnLoginOrLogout();
    void onBtnTest();
    
    std::function<void(const std::string& s)> showLog;

    //
    BCXTest();
    void init();
    void poolEvent();
    
    std::vector<std::string> functionList();
    void exec(const std::string& funName);
    
//    void login(const std::string &account, const std::string &pwd, const std::function<void(bool)> &callback);
//    void pay(const std::string &account_from,
//             const std::string &account_to,
//             const std::string &asset,
//             uint64_t amount);

    
    void test();
    void send(const std::string& data);
    
    std::string account = "test1";
    std::string pwd = "12345678";

    std::map<std::string, std::function<void(void)> > _testMap;
};
