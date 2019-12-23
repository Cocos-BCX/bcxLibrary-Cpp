#pragma once

#include "./types.hpp"

namespace bcx {

namespace protocol {

struct authority {

    authority(){}

    template<class ...Args>
    authority(uint32_t threshhold, Args... auths)
       : weight_threshold(threshhold) {
       add_authorities(auths...);
    }

    void add_authority( const public_key_type& k, weight_type w ) {
       key_auths[k] = w;
    }

    template<typename AuthType>
    void add_authorities(AuthType k, weight_type w) {
       add_authority(k, w);
    }

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

