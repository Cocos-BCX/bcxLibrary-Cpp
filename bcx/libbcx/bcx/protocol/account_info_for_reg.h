//
//  account_info_for_reg.h
//  bcx
//
//  Created by hugo on 2019/3/21.
//  Copyright © 2019 SDKBox. All rights reserved.
//

#ifndef account_info_for_reg_h
#define account_info_for_reg_h

#include "fc/reflect/reflect.hpp"

namespace bcx { namespace protocol {
    using namespace graphene::chain;
    
    struct account_info_for_reg_impl {
        std::string name;
        std::string owner_key;
        std::string active_key;
        std::string memo_key;
        std::string refcode;
        std::string referrer;
    };

    struct account_info_for_reg {
        account_info_for_reg_impl account;
    };
    
} }

FC_REFLECT( bcx::protocol::account_info_for_reg_impl,
           (name)
           (owner_key)
           (active_key)
           (memo_key)
           (refcode)
           (referrer)
           )

FC_REFLECT( bcx::protocol::account_info_for_reg,
           (account)
           )

#endif /* account_info_for_reg_h */
