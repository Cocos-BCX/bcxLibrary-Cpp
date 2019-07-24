//
//  nh_creat_world_view_operation.cpp
//  bcx
//
//  Created by hugo on 2019/3/29.
//  Copyright © 2019 SDKBox. All rights reserved.
//

#include "nh_create_world_view_operation.hpp"

namespace bcx { namespace protocol {
    
    graphene::chain::share_type nh_create_world_view_operation::calculate_fee(const fee_parameters_type& schedule )const {
        return schedule.fee;
    }
    
    void nh_create_world_view_operation::validate()const {
        FC_ASSERT( fee.amount >= 0 );
    }
    
} }

