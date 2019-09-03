//
//  contract_create_operation.hpp
//  bcx
//
//  Created by hugo on 2019/3/27.
//  Copyright © 2019 SDKBox. All rights reserved.
//

#pragma once

#include <graphene/chain/protocol/base.hpp>

namespace bcx { namespace protocol {
    
    struct contract_create_operation : public graphene::chain::base_operation {
        struct fee_parameters_type {
            uint64_t fee       = 20 * GRAPHENE_BLOCKCHAIN_PRECISION;
            uint32_t price_per_kbyte = 10 * GRAPHENE_BLOCKCHAIN_PRECISION; /// only required for large memos.
        };
        
        graphene::chain::asset fee;
        graphene::chain::account_id_type owner;
        std::string name;
        std::string data;
        graphene::chain::public_key_type contract_authority;
        graphene::chain::extensions_type extensions;
        
        graphene::chain::account_id_type    fee_payer()const { return owner; }
        void                                validate()const;
        graphene::chain::share_type         calculate_fee(const contract_create_operation::fee_parameters_type& k)const;
    };
    
} }

FC_REFLECT( bcx::protocol::contract_create_operation::fee_parameters_type,
           (fee)
           (price_per_kbyte)
           )

FC_REFLECT( bcx::protocol::contract_create_operation,
           (fee)
           (owner)
           (name)
           (data)
           (contract_authority)
           (extensions)
           )
