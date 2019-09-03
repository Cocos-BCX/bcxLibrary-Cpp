//
//  game_protocol_object.hpp
//  bcx
//
//  Created by hugo on 2019/4/2.
//  Copyright © 2019 SDKBox. All rights reserved.
//

#pragma once

#include <graphene/chain/protocol/base.hpp>
#include <graphene/chain/protocol/types.hpp>
#include <graphene/db/object.hpp>

namespace bcx { namespace protocol {
    
    class game_protocol_object : public graphene::db::abstract_object<contract_object> {
    public:
        static const uint8_t space_id = graphene::chain::protocol_ids;
        static const uint8_t type_id  = graphene::chain::contract_object_type;
        
        std::string dummy;
        graphene::chain::game_protocol_id_type get_id()const { return id; }
    };
    
} }

FC_REFLECT_DERIVED(bcx::protocol::game_protocol_object,
                   (graphene::db::object),
                   (dummy)
                   )

