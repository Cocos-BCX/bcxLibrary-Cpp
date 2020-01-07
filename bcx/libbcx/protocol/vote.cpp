#include "./vote.hpp"

namespace bcx {
namespace protocol {

} } // bcx::protocol

namespace fc {

void to_variant( const bcx::protocol::vote_id_type& var, fc::variant& vo, uint32_t max_depth ) {
   vo = string(var);
}

void from_variant( const fc::variant& var, bcx::protocol::vote_id_type& vo, uint32_t max_depth ) {
   vo = bcx::protocol::vote_id_type(var.as_string());
}

} // fc
