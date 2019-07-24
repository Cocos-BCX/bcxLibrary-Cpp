//
//  file_signature.cpp
//  bcx
//
//  Created by hugo on 2019/3/28.
//  Copyright © 2019 SDKBox. All rights reserved.
//

#include "file_signature.hpp"

namespace bcx { namespace protocol {
    
    graphene::chain::share_type file_signature::calculate_fee(const fee_parameters_type& schedule )const {
        return schedule.fee;
    }
    
    void file_signature::validate()const {
        FC_ASSERT( fee.amount >= 0 );
    }
    
} }

