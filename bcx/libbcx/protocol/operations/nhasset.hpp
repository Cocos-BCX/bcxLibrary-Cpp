#pragma once

#include "../types.hpp"
#include "../memo.hpp"
#include "../asset.hpp"

namespace bcx {

namespace protocol {

struct nh_asset_info {
    game_protocol_id_type id;
    std::string nh_hash;
    std::string base_describe;
    std::string nh_asset_creator;
    std::string nh_asset_owner;
    std::string nh_asset_active;
    std::string authority_account;
    std::string asset_qualifier;
    std::string world_view;
};

struct nh_register_asset_creator_operation {
    account_id_type fee_paying_account;
};

struct nh_create_world_view_operation {
    account_id_type fee_paying_account;
    std::string world_view;
};

struct nh_relate_world_view_operation {
    account_id_type related_account;
    account_id_type view_owner;
    std::string world_view;
};

struct nh_create_asset_operation {
    account_id_type fee_paying_account;
    account_id_type owner;
    std::string asset_id;
    std::string world_view;
    std::string base_describe;
};

struct nh_delete_asset_operation {
    account_id_type fee_paying_account;
    game_protocol_id_type nh_asset;
};

struct nh_transfer_asset_operation {
    account_id_type from;
    account_id_type to;
    game_protocol_id_type nh_asset;
};

struct nh_create_asset_order_operation {
    account_id_type seller;
    account_id_type otcaccount;
    asset pending_orders_fee;
    game_protocol_id_type nh_asset;
    std::string memo;
    asset price;
    fc::time_point_sec expiration;
};

struct nh_cancel_asset_order_operation {
    game_protocol_id_type order;
    account_id_type fee_paying_account;
    extensions_type extensions;
};

} // namespace protocol

} // namespace bcx


FC_REFLECT( bcx::protocol::nh_asset_info,
            (id)
            (nh_hash)
            (base_describe)
            (nh_asset_creator)
            (nh_asset_owner)
            (nh_asset_active)
            (authority_account)
            (asset_qualifier)
            (world_view)
)

FC_REFLECT( bcx::protocol::nh_register_asset_creator_operation,
           (fee_paying_account)
)

FC_REFLECT( bcx::protocol::nh_create_world_view_operation,
           (fee_paying_account)
           (world_view)
           )
FC_REFLECT( bcx::protocol::nh_relate_world_view_operation,
           (related_account)
           (world_view)
           (view_owner)
           )
FC_REFLECT( bcx::protocol::nh_create_asset_operation,
           (fee_paying_account)
           (owner)
           (asset_id)
           (world_view)
           (base_describe)
           )
FC_REFLECT( bcx::protocol::nh_delete_asset_operation,
           (fee_paying_account)
           (nh_asset)
           )
FC_REFLECT( bcx::protocol::nh_transfer_asset_operation,
           (from)
           (to)
           (nh_asset)
           )
FC_REFLECT( bcx::protocol::nh_create_asset_order_operation,
           (seller)
           (otcaccount)
           (pending_orders_fee)
           (nh_asset)
           (memo)
           (price)
           (expiration)
           )
FC_REFLECT( bcx::protocol::nh_cancel_asset_order_operation,
           (order)
           (fee_paying_account)
           (extensions)
           )

