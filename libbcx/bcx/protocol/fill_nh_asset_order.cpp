//
//  fill_nh_asset_order.cpp
//  bcx
//
//  Created by hugo on 2019/3/28.
//  Copyright © 2019 SDKBox. All rights reserved.
//

#include "fill_nh_asset_order.hpp"

namespace bcx { namespace protocol {
    
    graphene::chain::share_type fill_nh_asset_order::calculate_fee(const fee_parameters_type& schedule )const {
        return schedule.fee;
    }
    
    void fill_nh_asset_order::validate()const {
        FC_ASSERT( fee.amount >= 0 );
    }
    
} }

