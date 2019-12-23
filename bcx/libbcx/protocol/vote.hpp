#pragma once

#include "./types.hpp"

namespace bcx { namespace protocol {

struct vote_id_type {
   /// Lower 8 bits are type; upper 24 bits are instance
   uint32_t content;

   enum vote_type {
      committee,
      witness,
      worker,
      VOTE_TYPE_COUNT
   };

   /// Default constructor. Sets type and instance to 0
   vote_id_type():content(0){}
   /// Construct this vote_id_type with provided type and instance
   vote_id_type(vote_type type, uint32_t instance = 0)
      : content(instance<<8 | type)
   {}
   /// Construct this vote_id_type from a serial string in the form "type:instance"
   explicit vote_id_type(const std::string& serial)
   { try {
      auto colon = serial.find(':');
      FC_ASSERT( colon != std::string::npos );
      *this = vote_id_type(vote_type(std::stoul(serial.substr(0, colon))), std::stoul(serial.substr(colon+1)));
   } FC_CAPTURE_AND_RETHROW( (serial) ) }

   /// Set the type of this vote_id_type
   void set_type(vote_type type)
   {
      content &= 0xffffff00;
      content |= type & 0xff;
   }
   /// Get the type of this vote_id_type
   vote_type type()const
   {
      return vote_type(content & 0xff);
   }

   /// Set the instance of this vote_id_type
   void set_instance(uint32_t instance)
   {
      assert(instance < 0x01000000);
      content &= 0xff;
      content |= instance << 8;
   }
   /// Get the instance of this vote_id_type
   uint32_t instance()const
   {
      return content >> 8;
   }

   vote_id_type& operator =(vote_id_type other)
   {
      content = other.content;
      return *this;
   }
   /// Set the instance of this vote_id_type
   vote_id_type& operator =(uint32_t instance)
   {
      set_instance(instance);
      return *this;
   }
   /// Get the instance of this vote_id_type
   operator uint32_t()const
   {
      return instance();
   }

   /// Convert this vote_id_type to a serial string in the form "type:instance"
   explicit operator std::string()const
   {
      return std::to_string(type()) + ":" + std::to_string(instance());
   }
};

} }

namespace fc
{

class variant;

void to_variant( const bcx::protocol::vote_id_type& var, fc::variant& vo, uint32_t max_depth = 1 );
void from_variant( const fc::variant& var, bcx::protocol::vote_id_type& vo, uint32_t max_depth = 1 );

} // fc

FC_REFLECT_TYPENAME( fc::flat_set<bcx::protocol::vote_id_type> )

FC_REFLECT_ENUM( bcx::protocol::vote_id_type::vote_type, (witness)(committee)(worker)(VOTE_TYPE_COUNT) )
FC_REFLECT( bcx::protocol::vote_id_type, (content) )

