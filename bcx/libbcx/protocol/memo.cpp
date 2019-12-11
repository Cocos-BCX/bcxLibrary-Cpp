
#include "./memo.hpp"
#include "../fc/include/fc/crypto/aes.hpp"
#include "../fc/include/fc/crypto/sha224.hpp"

namespace bcx { namespace protocol {

void memo_data::set_message(const fc::ecc::private_key& priv, const fc::ecc::public_key& pub,
                            const std::string& msg, uint64_t custom_nonce) {
    if( priv != fc::ecc::private_key() && public_key_type(pub) != public_key_type() ) {
        from = priv.get_public_key();
        to = pub;
        if( custom_nonce == 0 ) {
            uint64_t entropy = fc::sha224::hash(fc::ecc::private_key::generate())._hash[0];
            entropy <<= 32;
            entropy                                                     &= 0xff00000000000000;
            nonce = (fc::time_point::now().time_since_epoch().count()   &  0x00ffffffffffffff) | entropy;
        } else {
            nonce = custom_nonce;
        }
        auto secret = priv.get_shared_secret(pub);
        auto nonce_plus_secret = fc::sha512::hash(fc::to_string(nonce) + secret.str());
        std::string text = memo_message(digest_type::hash(msg)._hash[0], msg).serialize();
        message = fc::aes_encrypt( nonce_plus_secret, std::vector<char>(text.begin(), text.end()) );
    } else {
        auto text = memo_message(0, msg).serialize();
        message = std::vector<char>(text.begin(), text.end());
    }
}

std::string memo_data::get_message(const fc::ecc::private_key& priv,
                                   const fc::ecc::public_key& pub)const {
    if( from != public_key_type() ) {
        auto secret = priv.get_shared_secret(pub);
        auto nonce_plus_secret = fc::sha512::hash(fc::to_string(nonce) + secret.str());
        auto plain_text = fc::aes_decrypt( nonce_plus_secret, message );
        auto result = memo_message::deserialize(std::string(plain_text.begin(), plain_text.end()));
        FC_ASSERT( result.checksum == uint32_t(digest_type::hash(result.text)._hash[0]) );
        return result.text;
    } else {
        return memo_message::deserialize(std::string(message.begin(), message.end())).text;
    }
}

std::string memo_message::serialize() const {
    auto serial_checksum = std::string(sizeof(checksum), ' ');
    (uint32_t&)(*serial_checksum.data()) = checksum;
    return serial_checksum + text;
}

memo_message memo_message::deserialize(const std::string& serial) {
    memo_message result;
    FC_ASSERT( serial.size() >= sizeof(result.checksum) );
    result.checksum = ((uint32_t&)(*serial.data()));
    result.text = serial.substr(sizeof(result.checksum));
    return result;
}

} } // bcx::protocol

