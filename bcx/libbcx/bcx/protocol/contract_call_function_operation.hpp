//
//  contract_call_function_operation.hpp
//  bcx
//
//  Created by hugo on 2019/3/29.
//  Copyright © 2019 SDKBox. All rights reserved.
//

#pragma once

#include <graphene/chain/protocol/base.hpp>
#include "bcx/lua_types.hpp"

namespace bcx { namespace protocol {
    
    struct contract_call_function_operation : public graphene::chain::base_operation {
        struct fee_parameters_type {
            uint64_t fee       = 20 * GRAPHENE_BLOCKCHAIN_PRECISION;
            uint32_t price_per_kbyte = 10 * GRAPHENE_BLOCKCHAIN_PRECISION; /// only required for large memos.
        };

        graphene::chain::asset fee;
        graphene::chain::account_id_type caller;
        graphene::chain::contract_id_type contract_id;
        std::string function_name;
        std::vector<bcx::lua_types> value_list;
        graphene::chain::extensions_type extensions;

        graphene::chain::account_id_type    fee_payer()const { return caller; }
        void                                validate()const;
        graphene::chain::share_type         calculate_fee(const fee_parameters_type& k)const;
    };
    
} }

FC_REFLECT( bcx::protocol::contract_call_function_operation::fee_parameters_type,
           (fee)
           (price_per_kbyte)
           )

FC_REFLECT( bcx::protocol::contract_call_function_operation,
           (caller)
           (contract_id)
           (function_name)
           (value_list)
           (extensions)
           )

