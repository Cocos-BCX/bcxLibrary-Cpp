#pragma once

#include <fc/config.hpp>
#include <deque>

namespace fc {

   namespace raw {
       template<typename Stream, typename T>
       void pack( Stream& s, const std::deque<T>& value, uint32_t _max_depth=FC_PACK_MAX_DEPTH );
       template<typename Stream, typename T>
       void unpack( Stream& s, std::deque<T>& value, uint32_t _max_depth=FC_PACK_MAX_DEPTH );
   }
} // namespace fc
