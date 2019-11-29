//
//  creat_nh_asset_order.hpp
//  bcx
//
//  Created by hugo on 2019/3/28.
//  Copyright © 2019 SDKBox. All rights reserved.
//

#pragma once

#include <graphene/chain/protocol/base.hpp>

namespace bcx { namespace protocol {
    
    struct nh_create_asset_order_operation : public graphene::chain::base_operation {
        struct fee_parameters_type {
            uint64_t fee       = 20 * GRAPHENE_BLOCKCHAIN_PRECISION;
            uint32_t price_per_kbyte = 10 * GRAPHENE_BLOCKCHAIN_PRECISION;
        };
        
        graphene::chain::asset fee;
        graphene::chain::account_id_type seller;
        graphene::chain::account_id_type otcaccount;
        graphene::chain::asset pending_orders_fee;
        graphene::chain::game_protocol_id_type nh_asset;
        std::string memo;
        graphene::chain::asset price;
        fc::time_point_sec expiration;
        
        graphene::chain::account_id_type    fee_payer()const { return seller; }
        void                                validate()const;
        graphene::chain::share_type         calculate_fee(const fee_parameters_type& k)const;
    };
    
} }

FC_REFLECT( bcx::protocol::nh_create_asset_order_operation::fee_parameters_type,
           (fee)
           (price_per_kbyte)
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


