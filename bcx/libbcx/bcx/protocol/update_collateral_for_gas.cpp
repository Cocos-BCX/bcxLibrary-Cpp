#include "update_collateral_for_gas.hpp"

namespace bcx { namespace protocol {
    
    graphene::chain::share_type update_collateral_for_gas::calculate_fee(const fee_parameters_type& schedule )const {
        return schedule.fee;
    }
    
    void update_collateral_for_gas::validate()const {
        FC_ASSERT( fee.amount >= 0 );
    }
    
} }
