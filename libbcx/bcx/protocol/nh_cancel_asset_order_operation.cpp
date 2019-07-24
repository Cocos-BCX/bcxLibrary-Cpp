//
//  cancel_nh_asset_order.cpp
//  bcx
//
//  Created by hugo on 2019/3/28.
//  Copyright © 2019 SDKBox. All rights reserved.
//

#include "nh_cancel_asset_order_operation.hpp"

namespace bcx { namespace protocol {
    
    graphene::chain::share_type nh_cancel_asset_order_operation::calculate_fee(const fee_parameters_type& schedule )const {
        return schedule.fee;
    }
    
    void nh_cancel_asset_order_operation::validate()const {
        FC_ASSERT( fee.amount >= 0 );
    }
    
} }

