#pragma once

#include <string>
#include "./object.hpp"
#include "../fc/include/fc/crypto/ripemd160.hpp"
#include "./types.hpp"

namespace bcx {

namespace protocol {

struct dynamic_global_property_object : public object {
    uint32_t          head_block_number = 0;
    block_id_type     head_block_id;
};

} // namespace protocol

} // namespace bcx


FC_REFLECT_DERIVED( bcx::protocol::dynamic_global_property_object, (bcx::protocol::object),
  (head_block_number)
  (head_block_id)
)

