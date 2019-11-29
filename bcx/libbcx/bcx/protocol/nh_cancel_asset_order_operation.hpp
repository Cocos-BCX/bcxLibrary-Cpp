//
//  cancel_nh_asset_order.hpp
//  bcx
//
//  Created by hugo on 2019/3/28.
//  Copyright © 2019 SDKBox. All rights reserved.
//
#pragma once

#include <graphene/chain/protocol/base.hpp>

namespace bcx { namespace protocol {
    
    struct nh_cancel_asset_order_operation : public graphene::chain::base_operation {
        struct fee_parameters_type {
            uint64_t fee       = 20 * GRAPHENE_BLOCKCHAIN_PRECISION;
            uint32_t price_per_kbyte = 10 * GRAPHENE_BLOCKCHAIN_PRECISION;
        };
        
        graphene::chain::asset fee;
        graphene::chain::game_protocol_id_type order;
        graphene::chain::account_id_type fee_paying_account;
        graphene::chain::extensions_type extensions;
        
        graphene::chain::account_id_type    fee_payer()const { return fee_paying_account; }
        void                                validate()const;
        graphene::chain::share_type         calculate_fee(const fee_parameters_type& k)const;
    };
    
} }

FC_REFLECT( bcx::protocol::nh_cancel_asset_order_operation::fee_parameters_type,
           (fee)
           (price_per_kbyte)
           )

FC_REFLECT( bcx::protocol::nh_cancel_asset_order_operation,
           (order)
           (fee_paying_account)
           (extensions)
           )

