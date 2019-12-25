#pragma once

#include "../fc/include/fc/io/varint.hpp"
#include "../fc/include/fc/exception/exception.hpp"

#define BCX_MAX_INSTANCE_ID  (uint64_t(-1)>>16)

namespace bcx {
namespace protocol {

struct object_id_type {
   object_id_type( uint8_t s, uint8_t t, uint64_t i ) {
      number = (uint64_t(s)<<56) | (uint64_t(t)<<48) | i;
   }
   object_id_type(){ number = 0; }

   uint8_t  space()const       { return number >> 56;              }
   uint8_t  type()const        { return number >> 48 & 0x00ff;     }
   uint16_t space_type()const { return number >> 48;              }
   uint64_t instance()const { return number & BCX_MAX_INSTANCE_ID; }
   bool     is_null()const { return number == 0; }
   explicit operator uint64_t()const { return number; }

   friend bool  operator == ( const object_id_type& a, const object_id_type& b ) { return a.number == b.number; }
   friend bool  operator != ( const object_id_type& a, const object_id_type& b ) { return a.number != b.number; }
   friend bool  operator < ( const object_id_type& a, const object_id_type& b ) { return a.number < b.number; }
   friend bool  operator > ( const object_id_type& a, const object_id_type& b ) { return a.number > b.number; }

   explicit operator std::string() const {
       return fc::to_string(space()) + "." + fc::to_string(type()) + "." + fc::to_string(instance());
   }

   uint64_t                   number;
};

class object;

template<uint8_t SpaceID, uint8_t TypeID, typename T = object>
struct object_id {
    typedef T type;
    static const uint8_t space_id = SpaceID;
    static const uint8_t type_id = TypeID;

    object_id(){}
    object_id( fc::unsigned_int i ):instance(i){}
    explicit object_id( uint64_t i ):instance(i){}
    object_id( object_id_type id ):instance(id.instance()){}

    operator object_id_type()const { return object_id_type( SpaceID, TypeID, instance.value ); }
    explicit operator uint64_t()const { return object_id_type( *this ).number; }

    // for std::set
//    bool operator< (const object_id & objId) const {
//        if (space_id != objId.space_id) {
//            return space_id < objId.space_id;
//        }
//        if (type_id != objId.type_id) {
//            return type_id < objId.type_id;
//        }
//        return instance < objId.instance;
//    }

    friend bool  operator == ( const object_id& a, const object_id& b ) { return a.instance == b.instance; }
    friend bool  operator != ( const object_id& a, const object_id& b ) { return a.instance != b.instance; }
    friend bool  operator == ( const object_id_type& a, const object_id& b ) { return a == object_id_type(b); }
    friend bool  operator != ( const object_id_type& a, const object_id& b ) { return a != object_id_type(b); }
    friend bool  operator == ( const object_id& b, const object_id_type& a ) { return a == object_id_type(b); }
    friend bool  operator != ( const object_id& b, const object_id_type& a ) { return a != object_id_type(b); }

    friend bool  operator < ( const object_id& a, const object_id& b ) { return a.instance.value < b.instance.value; }
    friend bool  operator > ( const object_id& a, const object_id& b ) { return a.instance.value > b.instance.value; }

    friend size_t hash_value( object_id v ) { return std::hash<uint64_t>()(v.instance.value); }
    
    //unsigned_int instance;
    fc::unsigned_int64 instance;
};

} // namespace protocol
} // namespace bcx

FC_REFLECT( bcx::protocol::object_id_type, (number) )

// REFLECT object_id manually because it has 2 template params
namespace fc {

template<uint8_t SpaceID, uint8_t TypeID, typename T>
struct get_typename<bcx::protocol::object_id<SpaceID,TypeID,T>> {
   static const char* name() {
      return typeid(get_typename).name();
      static std::string _str = string("bcx::protocol::object_id<")+fc::to_string(SpaceID) + ":" + fc::to_string(TypeID)+">";
      return _str.c_str();
   }
};

template<uint8_t SpaceID, uint8_t TypeID, typename T>
struct reflector<bcx::protocol::object_id<SpaceID,TypeID,T> > {
    typedef bcx::protocol::object_id<SpaceID,TypeID,T> type;
    typedef fc::true_type  is_defined;
    typedef fc::false_type is_enum;
    enum  member_count_enum {
        local_member_count = 1,
        total_member_count = 1
    };
    template<typename Visitor>
    static inline void visit( const Visitor& visitor ) {
        typedef decltype(((type*)nullptr)->instance) member_type;
        visitor.TEMPLATE operator()<member_type,type,&type::instance>( "instance" );
    }
};


inline void to_variant( const bcx::protocol::object_id_type& var,  fc::variant& vo, uint32_t max_depth = 1 ) {
    vo = std::string( var );
}

inline void from_variant( const fc::variant& var,  bcx::protocol::object_id_type& vo, uint32_t max_depth = 1 ) {
    try {
        vo.number = 0;
        const auto& s = var.get_string();
        auto first_dot = s.find('.');
        auto second_dot = s.find('.',first_dot+1);
        FC_ASSERT( first_dot != second_dot );
        FC_ASSERT( first_dot != 0 && first_dot != std::string::npos );
        vo.number = fc::to_uint64(s.substr( second_dot+1 ));
        FC_ASSERT( vo.number <= BCX_MAX_INSTANCE_ID );
        auto space_id = fc::to_uint64( s.substr( 0, first_dot ) );
        FC_ASSERT( space_id <= 0xff );
        auto type_id =  fc::to_uint64( s.substr( first_dot+1, second_dot-first_dot-1 ) );
        FC_ASSERT( type_id <= 0xff );
        vo.number |= (space_id << 56) | (type_id << 48);
    } FC_CAPTURE_AND_RETHROW( (var) )
}

template<uint8_t SpaceID, uint8_t TypeID, typename T>
void to_variant( const bcx::protocol::object_id<SpaceID,TypeID,T>& var,  fc::variant& vo, uint32_t max_depth = 1 ) {
    vo = fc::to_string(SpaceID) + "." + fc::to_string(TypeID) + "." + fc::to_string(var.instance.value);
}

template<uint8_t SpaceID, uint8_t TypeID, typename T>
void from_variant( const fc::variant& var,  bcx::protocol::object_id<SpaceID,TypeID,T>& vo, uint32_t max_depth = 1 ) {
    try {
        const auto& s = var.get_string();
        auto first_dot = s.find('.');
        auto second_dot = s.find('.',first_dot+1);
        FC_ASSERT( first_dot != second_dot );
        FC_ASSERT( first_dot != 0 && first_dot != std::string::npos );
        FC_ASSERT( fc::to_uint64( s.substr( 0, first_dot ) ) == SpaceID &&
            fc::to_uint64( s.substr( first_dot+1, second_dot-first_dot-1 ) ) == TypeID,
                      "Space.Type.0 (${SpaceID}.${TypeID}.0) doesn't match expected value ${var}",
                      ("TypeID",TypeID)("SpaceID",SpaceID)("var",var) );
        vo.instance = fc::to_uint64(s.substr( second_dot+1 ));
    } FC_CAPTURE_AND_RETHROW( (var) )
}

} // namespace fc


