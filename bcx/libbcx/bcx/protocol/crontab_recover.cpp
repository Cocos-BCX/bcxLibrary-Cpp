#include "crontab_recover.hpp"

namespace bcx { namespace protocol {
    
    graphene::chain::share_type crontab_recover::calculate_fee(const fee_parameters_type& schedule )const {
        return schedule.fee;
    }
    
    void crontab_recover::validate()const {
        FC_ASSERT( fee.amount >= 0 );
    }
    
} }
