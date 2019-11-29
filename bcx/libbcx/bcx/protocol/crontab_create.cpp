#include "crontab_create.hpp"

namespace bcx { namespace protocol {
    
    graphene::chain::share_type crontab_create::calculate_fee(const fee_parameters_type& schedule )const {
        return schedule.fee;
    }
    
    void crontab_create::validate()const {
        FC_ASSERT( fee.amount >= 0 );
    }
    
} }
