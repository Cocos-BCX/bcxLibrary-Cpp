//
//  relate_parent_file.cpp
//  bcx
//
//  Created by hugo on 2019/3/28.
//  Copyright © 2019 SDKBox. All rights reserved.
//

#include "relate_parent_file.hpp"

namespace bcx { namespace protocol {

    graphene::chain::share_type relate_parent_file::calculate_fee(const fee_parameters_type& schedule )const {
        return schedule.fee;
    }
    
    void relate_parent_file::validate()const {
        FC_ASSERT( fee.amount >= 0 );
    }
    
} }

