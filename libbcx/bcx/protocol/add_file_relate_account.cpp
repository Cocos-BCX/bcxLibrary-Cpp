//
//  add_file_relate_account.cpp
//  bcx
//
//  Created by hugo on 2019/3/28.
//  Copyright © 2019 SDKBox. All rights reserved.
//

#include "add_file_relate_account.hpp"

namespace bcx { namespace protocol {
    
    graphene::chain::share_type add_file_relate_account::calculate_fee(const fee_parameters_type& schedule )const {
        return schedule.fee;
    }
    
    void add_file_relate_account::validate()const {
        FC_ASSERT( fee.amount >= 0 );
    }
    
} }

