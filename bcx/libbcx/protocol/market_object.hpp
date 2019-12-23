#pragma once

#include "./types.hpp"

#pragma once

namespace bcx {

namespace protocol {

class limit_order_object : public abstract_object<limit_order_object> {
public:
    static const uint8_t space_id = protocol_ids;
    static const uint8_t type_id  = limit_order_object_type;

    fc::time_point_sec   expiration;
    account_id_type  seller;
    share_type       for_sale; ///< asset id is sell_price.base.asset_id
    price            sell_price;
    share_type       deferred_fee; ///< fee converted to CORE
    asset            deferred_paid_fee; ///< originally paid fee
};


} // namespace protocol

} // namespace bcx


FC_REFLECT_DERIVED( bcx::protocol::limit_order_object, (graphene::db::object),
                   (expiration)(seller)(for_sale)(sell_price)(deferred_fee)(deferred_paid_fee)
)

