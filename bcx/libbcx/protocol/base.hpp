#pragma once

#include <set>
#include "../fc/include/fc/static_variant.hpp"

namespace bcx {

namespace protocol {

/**
 *  For future expansion many structus include a single member of type
 *  extensions_type that can be changed when updating a protocol.  You can
 *  always add new types to a static_variant without breaking backward
 *  compatibility.
 */
typedef fc::static_variant<void_t>      future_extensions;

/**
 *  A flat_set is used to make sure that only one extension of
 *  each type is added and that they are added in order.
 *
 *  @note static_variant compares only the type tag and not the
 *  content.
 */
typedef std::set<future_extensions> extensions_type;

} // namespace protocol

} // namespace bcx

