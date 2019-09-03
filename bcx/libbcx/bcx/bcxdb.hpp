//
//  bcxdb.hpp
//  bcx
//
//  Created by hugo on 2019/3/13.
//  Copyright © 2019 SDKBox. All rights reserved.
//

#ifndef bcxdb_hpp
#define bcxdb_hpp

#include <vector>

#include "platform/PlatformMacros.h"
#include "graphene/chain/global_property_object.hpp"
#include "./protocol/full_account.hpp"

NS_BCX_BEGIN

class bcxdb {
public:
    int dbAPIId = 0;
    int networkAPIId = 0;
    int historyAPIId = 0;
    std::string chain_id;
    graphene::chain::dynamic_global_property_object dgpo;
    bcx::protocol::full_account current_full_account;
    std::map<std::string, fc::ecc::private_key> savedKeys;
};

NS_BCX_END

#endif /* bcxdb_hpp */
