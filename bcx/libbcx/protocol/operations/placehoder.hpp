#pragma once

#include "../types.hpp"
#include "../memo.hpp"
#include "../asset.hpp"

namespace bcx {

namespace protocol {

//empty, just take place hodler

struct ph_operation {};

struct asset_update_restricted {};
struct asset_update_bitasset_operation {};
struct asset_update_feed_producers_operation {};


} // namespace protocol

} // namespace bcx

FC_REFLECT( bcx::protocol::ph_operation, )
FC_REFLECT( bcx::protocol::asset_update_restricted, )
FC_REFLECT( bcx::protocol::asset_update_bitasset_operation, )
FC_REFLECT( bcx::protocol::asset_update_feed_producers_operation, )
