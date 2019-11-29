#include "temporary_authority_chang.hpp"

namespace bcx { namespace protocol {
    
    graphene::chain::share_type temporary_authority_chang::calculate_fee(const fee_parameters_type& schedule )const {
        return schedule.fee;
    }
    
    void temporary_authority_chang::validate()const {
        FC_ASSERT( fee.amount >= 0 );
    }
    
} }
