#pragma once

#include "./types.hpp"

namespace bcx {

namespace protocol {

struct authority {
    uint32_t                              weight_threshold = 0;
//    std::map<account_id_type,weight_type> account_auths;
    std::map<public_key_type, weight_type> key_auths;
};

} // namespace protocol

} // namespace bcx

FC_REFLECT( bcx::protocol::authority,
           (weight_threshold)
           (key_auths)
           )

