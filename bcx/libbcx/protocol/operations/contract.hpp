#pragma once

#include "../types.hpp"
#include "../memo.hpp"
#include "../asset.hpp"
#include "../lua_types.hpp"


namespace bcx {

namespace protocol {

struct contract_create_operation {
    account_id_type owner;
    std::string name;
    std::string data;
    public_key_type contract_authority;
    extensions_type extensions;
};

struct contract_revise_operation {
    account_id_type reviser;
    contract_id_type contract_id;
    std::string data;
    extensions_type extensions;
};

struct contract_call_function_operation {
    account_id_type caller;
    contract_id_type contract_id;
    std::string function_name;
    std::vector<lua_types> value_list;
    extensions_type extensions;
};

} // namespace protocol

} // namespace bcx

FC_REFLECT( bcx::protocol::contract_create_operation,
            (owner)
            (name)
            (data)
            (contract_authority)
            (extensions)
)

FC_REFLECT( bcx::protocol::contract_revise_operation,
            (reviser)
            (contract_id)
            (data)
            (extensions)
)

FC_REFLECT( bcx::protocol::contract_call_function_operation,
            (caller)
            (contract_id)
            (function_name)
            (value_list)
            (extensions)
)

