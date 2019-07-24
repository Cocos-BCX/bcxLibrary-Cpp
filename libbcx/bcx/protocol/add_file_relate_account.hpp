//
//  add_file_relate_account.hpp
//  bcx
//
//  Created by hugo on 2019/3/28.
//  Copyright © 2019 SDKBox. All rights reserved.
//
#pragma once

#include <graphene/chain/protocol/base.hpp>

namespace bcx { namespace protocol {
    
    struct add_file_relate_account : public graphene::chain::base_operation {
        struct fee_parameters_type {
            uint64_t fee       = 20 * GRAPHENE_BLOCKCHAIN_PRECISION;
            uint32_t price_per_kbyte = 10 * GRAPHENE_BLOCKCHAIN_PRECISION;
        };
        
        graphene::chain::asset fee;
        graphene::chain::account_id_type owner;
        
        graphene::chain::account_id_type    fee_payer()const { return owner; }
        void                                validate()const;
        graphene::chain::share_type         calculate_fee(const fee_parameters_type& k)const;
    };
    
} }

FC_REFLECT( bcx::protocol::add_file_relate_account::fee_parameters_type,
           (fee)
           (price_per_kbyte)
           )

FC_REFLECT( bcx::protocol::add_file_relate_account,
           (fee)
           (owner)
           )

