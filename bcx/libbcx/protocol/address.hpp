#pragma once

#include <string>
#include <vector>
#include "./config.hpp"
#include "../fc/include/fc/crypto/ripemd160.hpp"

namespace bcx {
namespace protocol {

class address {
public:
    address();
    explicit address( const std::string& base58str );

    static bool is_valid( const std::string& base58str, const std::string& prefix = BCX_ADDRESS_PREFIX );

    explicit operator std::string()const; ///< converts to base58 + checksum

    fc::ripemd160 addr;
};

inline bool operator == ( const address& a, const address& b ) { return a.addr == b.addr; }
inline bool operator != ( const address& a, const address& b ) { return a.addr != b.addr; }
inline bool operator <  ( const address& a, const address& b ) { return a.addr <  b.addr; }

} // namespace protocol
} // namespace bcx


namespace fc {
   void to_variant( const bcx::protocol::address& var,  fc::variant& vo, uint32_t max_depth = 1 );
   void from_variant( const fc::variant& var,  bcx::protocol::address& vo, uint32_t max_depth = 1 );
}

namespace std {
   template<>
   struct hash<bcx::protocol::address> {
       public:
         size_t operator()(const bcx::protocol::address &a) const {
            return (uint64_t(a.addr._hash[0])<<32) | uint64_t( a.addr._hash[0] );
         }
   };
}

#include "../fc/include/fc/reflect/reflect.hpp"
FC_REFLECT( bcx::protocol::address, (addr) )
