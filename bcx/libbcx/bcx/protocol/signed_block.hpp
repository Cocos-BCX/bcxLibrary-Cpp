#pragma once

#include <graphene/chain/protocol/block.hpp>

namespace bcx { namespace protocol {
    using namespace graphene::chain;

    struct signed_block : public signed_block_header
    {
       checksum_type calculate_merkle_root()const;
       std::vector<std::pair<std::string, graphene::chain::processed_transaction>> transactions;
    };

} }

FC_REFLECT_DERIVED( bcx::protocol::signed_block, (graphene::chain::signed_block_header), (transactions))


