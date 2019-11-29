//
//  contract_revise_operation.hpp
//  bcx
//
//  Created by hugo on 2019/3/28.
//  Copyright © 2019 SDKBox. All rights reserved.
//

#pragma once

#include <graphene/chain/protocol/base.hpp>

namespace bcx { namespace protocol {
    
    struct contract_revise_operation : public graphene::chain::base_operation {
        struct fee_parameters_type {
            uint64_t fee       = 20 * GRAPHENE_BLOCKCHAIN_PRECISION;
            uint32_t price_per_kbyte = 10 * GRAPHENE_BLOCKCHAIN_PRECISION; /// only required for large memos.
        };
        
        graphene::chain::asset fee;
        graphene::chain::account_id_type reviser;
        graphene::chain::contract_id_type contract_id;
        std::string data;
        graphene::chain::extensions_type extensions;
        
        graphene::chain::account_id_type    fee_payer()const { return reviser; }
        void                                validate()const;
        graphene::chain::share_type         calculate_fee(const fee_parameters_type& k)const;
    };
    
} }

FC_REFLECT( bcx::protocol::contract_revise_operation::fee_parameters_type,
           (fee)
           (price_per_kbyte)
           )

FC_REFLECT( bcx::protocol::contract_revise_operation,
           (reviser)
           (contract_id)
           (data)
           (extensions)
           )


