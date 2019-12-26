#pragma once

#include "./types.hpp"

#pragma once

namespace bcx {

namespace protocol {

class game_protocol_object : public abstract_object<game_protocol_object> {
public:
    static const uint8_t space_id = game_protocol_ids;
    static const uint8_t type_id  = game_developer_object_type;

    game_protocol_id_type get_id()const { return id; }
};


} // namespace protocol

} // namespace bcx


FC_REFLECT_DERIVED(bcx::protocol::game_protocol_object, (graphene::db::object), )

