#include "asset_update_restricted.hpp"

namespace bcx { namespace protocol {
    
    graphene::chain::share_type asset_update_restricted::calculate_fee(const fee_parameters_type& schedule )const {
        return schedule.fee;
    }
    
    void asset_update_restricted::validate()const {
        FC_ASSERT( fee.amount >= 0 );
    }
    
} }
