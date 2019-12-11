#pragma once

#include <string>
#include <vector>
#include "../fc/include/fc/reflect/reflect.hpp"
#include "../fc/include/fc/variant.hpp"
#include "./object_id.hpp"

namespace bcx {

namespace protocol {

struct object {
    object_id_type id;
    
//    virtual unique_ptr<object> clone()const = 0;
//    virtual void               move_from( object& obj ) = 0;
//    virtual variant            to_variant()const  = 0;
//    virtual vector<char>       pack()const = 0;
//    virtual fc::uint128        hash()const = 0;

};

template<typename DerivedClass>
class abstract_object : public object {
public:
//    virtual unique_ptr<object> clone()const {
//        return unique_ptr<object>(new DerivedClass( *static_cast<const DerivedClass*>(this) ));
//    }
//
//    virtual void move_from( object& obj ) {
//        static_cast<DerivedClass&>(*this) = std::move( static_cast<DerivedClass&>(obj) );
//    }
//    virtual variant to_variant()const { return variant( static_cast<const DerivedClass&>(*this), MAX_NESTING ); }
//    virtual vector<char> pack()const  { return fc::raw::pack( static_cast<const DerivedClass&>(*this) ); }
//    virtual fc::uint128  hash()const  {
//        auto tmp = this->pack();
//        return fc::city_hash_crc_128( tmp.data(), tmp.size() );
//    }
};

} // namespace protocol

} // namespace bcx



FC_REFLECT( bcx::protocol::object, (id) )

