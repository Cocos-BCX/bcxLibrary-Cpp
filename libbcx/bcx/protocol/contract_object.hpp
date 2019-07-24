//
//  contract_object.hpp
//  bcx
//
//  Created by hugo on 2019/3/28.
//  Copyright © 2019 SDKBox. All rights reserved.
//

#pragma once

#include <graphene/chain/protocol/base.hpp>
#include <graphene/chain/protocol/types.hpp>
#include <graphene/db/object.hpp>

namespace bcx { namespace protocol {
    
    class contract_object : public graphene::db::abstract_object<contract_object> {
    public:
        static const uint8_t space_id = graphene::chain::protocol_ids;
        static const uint8_t type_id  = graphene::chain::contract_object_type;

        graphene::chain::contract_id_type get_id()const { return id; }
        
        fc::time_point_sec creation_date;
        graphene::chain::account_id_type owner;
        std::string name;
        std::string current_version;
        std::string contract_authority;
        bool check_contract_authority;
//        std::vector<variant>        contract_data;
//        std::vector<contract_abi>   contract_ABI;
//        std::string lua_code_b;
        std::string lua_code;
    };

} }

FC_REFLECT_DERIVED( bcx::protocol::contract_object,
                   (graphene::db::object),
                   (creation_date)(owner)(name)(current_version)(contract_authority)(check_contract_authority)(lua_code) )


