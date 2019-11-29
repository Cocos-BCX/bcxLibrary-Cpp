//
//  nh_register_creator_operation.cpp
//  bcx
//
//  Created by hugo on 2019/3/29.
//  Copyright © 2019 SDKBox. All rights reserved.
//

#include "nh_register_asset_creator_operation.hpp"

namespace bcx { namespace protocol {
    
    graphene::chain::share_type nh_register_asset_creator_operation::calculate_fee(const fee_parameters_type& schedule )const {
        return schedule.fee;
    }
    
    void nh_register_asset_creator_operation::validate()const {
        FC_ASSERT( fee.amount >= 0 );
    }

} }

