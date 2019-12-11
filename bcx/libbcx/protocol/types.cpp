#include "./config.hpp"
#include "./types.hpp"
#include "../fc/include/fc/io/raw.hpp"

namespace bcx {

namespace protocol {

public_key_type::public_key_type():key_data(){};

public_key_type::public_key_type( const fc::ecc::public_key_data& data )
    :key_data( data ) {};

public_key_type::public_key_type( const fc::ecc::public_key& pubkey )
    :key_data( pubkey ) {};

public_key_type::public_key_type( const std::string& base58str ) {
  // TODO:  Refactor syntactic checks into static is_valid()
  //        to make public_key_type API more similar to address API
   std::string prefix( BCX_ADDRESS_PREFIX );
   const size_t prefix_len = prefix.size();
   FC_ASSERT( base58str.size() > prefix_len );
   FC_ASSERT( base58str.substr( 0, prefix_len ) ==  prefix , "", ("base58str", base58str) );
   auto bin = fc::from_base58( base58str.substr( prefix_len ) );
   auto bin_key = fc::raw::unpack<binary_key>(bin);
   key_data = bin_key.data;
   FC_ASSERT( fc::ripemd160::hash( key_data.data, key_data.size() )._hash[0] == bin_key.check );
};

public_key_type::operator fc::ecc::public_key_data() const {
   return key_data;
};

public_key_type::operator fc::ecc::public_key() const {
   return fc::ecc::public_key( key_data );
};

public_key_type::operator std::string() const {
   binary_key k;
   k.data = key_data;
   k.check = fc::ripemd160::hash( k.data.data, k.data.size() )._hash[0];
   auto data = fc::raw::pack( k );
   return BCX_ADDRESS_PREFIX + fc::to_base58( data.data(), data.size() );
}

bool public_key_type::operator < (const public_key_type & cmp) const {
    const std::string a = std::string((*this));
    const std::string b = std::string(cmp);

    return (a < b);
}

bool operator == ( const public_key_type& p1, const fc::ecc::public_key& p2) {
   return p1.key_data == p2.serialize();
}

bool operator == ( const public_key_type& p1, const public_key_type& p2) {
   return p1.key_data == p2.key_data;
}

bool operator != ( const public_key_type& p1, const public_key_type& p2) {
   return p1.key_data != p2.key_data;
}

} // namespace protocol

} // namespace bcx


namespace fc {

void to_variant( const bcx::protocol::public_key_type& var,  fc::variant& vo, uint32_t max_depth) {
    vo = std::string(var);
}

void from_variant( const fc::variant& var,  bcx::protocol::public_key_type& vo, uint32_t max_depth) {
    vo = bcx::protocol::public_key_type(var.as_string());
}

}

