//
//  contract_create_operation.cpp
//  bcx
//
//  Created by hugo on 2019/3/27.
//  Copyright © 2019 SDKBox. All rights reserved.
//

#include "./contract_create_operation.hpp"

namespace bcx { namespace protocol {

    graphene::chain::share_type contract_create_operation::calculate_fee(
        const contract_create_operation::fee_parameters_type& schedule )const {
    return schedule.fee;
}


void contract_create_operation::validate()const {
    FC_ASSERT( fee.amount >= 0 );
}

} }

