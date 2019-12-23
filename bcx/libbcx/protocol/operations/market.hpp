#pragma once

#include "../types.hpp"
#include "../memo.hpp"
#include "../asset.hpp"
#include "../ext.hpp"

namespace bcx {

namespace protocol {

struct limit_order_create_operation {
    account_id_type seller;
    asset           amount_to_sell;
    asset           min_to_receive;
    fc::time_point_sec expiration = fc::time_point_sec::maximum();
    bool fill_or_kill = false;
    extensions_type   extensions;
};

struct limit_order_cancel_operation {
    limit_order_id_type order;
    /** must be order->seller */
    account_id_type     fee_paying_account;
    extensions_type   extensions;
};

struct call_order_update_operation {
    
    struct options_type {
       fc::optional<uint16_t> target_collateral_ratio; ///< maximum CR to maintain when selling collateral on margin call
    };

    account_id_type     funding_account; ///< pays fee, collateral, and cover
    asset               delta_collateral; ///< the amount of collateral to add to the margin position
    asset               delta_debt; ///< the amount of the debt to be paid off, may be negative to issue new debt

    typedef extension<options_type> extensions_type; // note: this will be jsonified to {...} but no longer [...]
    extensions_type     extensions;
};

struct fill_order_operation {
    object_id_type      order_id;
    account_id_type     account_id;
    asset               pays;
    asset               receives;
    price               fill_price;
    bool                is_maker;
};

} // namespace protocol

} // namespace bcx


FC_REFLECT( bcx::protocol::call_order_update_operation::options_type,
           (target_collateral_ratio)
)

FC_REFLECT( bcx::protocol::limit_order_create_operation,
            (seller)(amount_to_sell)(min_to_receive)(expiration)(fill_or_kill)(extensions)
)

FC_REFLECT( bcx::protocol::limit_order_cancel_operation,
            (fee_paying_account)(order)(extensions)
)

FC_REFLECT( bcx::protocol::call_order_update_operation,
            (funding_account)(delta_collateral)(delta_debt)(extensions)
)

FC_REFLECT( bcx::protocol::fill_order_operation,
            (order_id)(account_id)(pays)(receives)(fill_price)(is_maker)
)

