#include "./address.hpp"
#include "../fc/include/fc/crypto/base58.hpp"

namespace bcx {
namespace protocol {

address::address(){}

address::address( const std::string& base58str ) {
    std::string prefix( BCX_ADDRESS_PREFIX );
    FC_ASSERT( is_valid( base58str, prefix ), "${str}", ("str",base58str) );

    std::vector<char> v = fc::from_base58( base58str.substr( prefix.size() ) );
    memcpy( (char*)addr._hash, v.data(), std::min<size_t>( v.size()-4, sizeof( addr ) ) );
}

bool address::is_valid( const std::string& base58str, const std::string& prefix ) {
    const size_t prefix_len = prefix.size();
    if( base58str.size() <= prefix_len )
        return false;
    if( base58str.substr( 0, prefix_len ) != prefix )
        return false;
    std::vector<char> v;
    try {
        v = fc::from_base58( base58str.substr( prefix_len ) );
    } catch( const fc::parse_error_exception& e ) {
        return false;
    }

    if( v.size() != sizeof( fc::ripemd160 ) + 4 )
        return false;

    const fc::ripemd160 checksum = fc::ripemd160::hash( v.data(), v.size() - 4 );
    if( memcmp( v.data() + 20, (char*)checksum._hash, 4 ) != 0 )
        return false;

    return true;
}

address::operator std::string()const {
    fc::array<char,24> bin_addr;
    memcpy( (char*)&bin_addr, (char*)&addr, sizeof( addr ) );
    auto checksum = fc::ripemd160::hash( (char*)&addr, sizeof( addr ) );
    memcpy( ((char*)&bin_addr)+20, (char*)&checksum._hash[0], 4 );
    return BCX_ADDRESS_PREFIX + fc::to_base58( bin_addr.data, sizeof( bin_addr ) );
}

} // namespace protocol
} // namespace bcx

namespace fc {

void to_variant( const bcx::protocol::address& var,  variant& vo, uint32_t max_depth ) {
    vo = std::string(var);
}
void from_variant( const variant& var,  bcx::protocol::address& vo, uint32_t max_depth ) {
    vo = bcx::protocol::address( var.as_string() );
}

}
