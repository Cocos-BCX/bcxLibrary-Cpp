#pragma once

#include "../fc/include/fc/reflect/reflect.hpp"
#include "../fc/include/fc/static_variant.hpp"
#include "./types.hpp"

namespace bcx {

namespace protocol {

struct no_special_authority {};

struct top_holders_special_authority {
   asset_id_type asset;
   uint8_t       num_top_holders = 1;
};

typedef fc::static_variant<
    no_special_authority,
    top_holders_special_authority
> special_authority;

} // namespace protocol

} // namespace bcx


FC_REFLECT( bcx::protocol::no_special_authority, )
FC_REFLECT( bcx::protocol::top_holders_special_authority, (asset)(num_top_holders) )
FC_REFLECT_TYPENAME( bcx::protocol::special_authority )

