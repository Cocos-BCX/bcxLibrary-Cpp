//
//  contract_call_function_operation.cpp
//  bcx
//
//  Created by hugo on 2019/3/29.
//  Copyright © 2019 SDKBox. All rights reserved.
//

#include "contract_call_function_operation.hpp"

namespace bcx { namespace protocol {
    
    graphene::chain::share_type contract_call_function_operation::calculate_fee(const fee_parameters_type& schedule )const {
        return schedule.fee;
    }

    void contract_call_function_operation::validate()const {
        FC_ASSERT( fee.amount >= 0 );
    }
    
} }

