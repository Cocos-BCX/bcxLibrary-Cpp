//
//  relate_world_view.hpp
//  bcx
//
//  Created by hugo on 2019/3/28.
//  Copyright © 2019 SDKBox. All rights reserved.
//

#pragma once

#include <graphene/chain/protocol/base.hpp>

namespace bcx { namespace protocol {
    
    struct nh_relate_world_view_operation : public graphene::chain::base_operation {
        struct fee_parameters_type {
            uint64_t fee       = 20 * GRAPHENE_BLOCKCHAIN_PRECISION;
            uint32_t price_per_kbyte = 10 * GRAPHENE_BLOCKCHAIN_PRECISION;
        };
        
        graphene::chain::asset fee;
        graphene::chain::account_id_type related_account;
        graphene::chain::account_id_type view_owner;
        std::string world_view;

        graphene::chain::account_id_type    fee_payer()const { return view_owner; }
        void                                validate()const;
        graphene::chain::share_type         calculate_fee(const fee_parameters_type& k)const;
    };
    
} }

FC_REFLECT( bcx::protocol::nh_relate_world_view_operation::fee_parameters_type,
           (fee)
           (price_per_kbyte)
           )

FC_REFLECT( bcx::protocol::nh_relate_world_view_operation,
           (related_account)
           (world_view)
           (view_owner)
           )


