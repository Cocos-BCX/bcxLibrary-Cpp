#pragma once

//#include <boost/preprocessor/seq/for_each.hpp>
#include "../fc/include/fc/static_variant.hpp"
#include "../fc/include/fc/reflect/variant.hpp"

using namespace std;

namespace bcx { namespace protocol {

#define LUATYPE(T) \
typedef struct lua_##T { \
    T v; \
    lua_##T(T v) { \
        this->v = v;  \
    } \
    lua_##T() {} \
} lua_##T; \

#define LUATYPE_NAME(T) lua_##T

struct lua_table;
struct lua_key;

LUATYPE(bool)
LUATYPE(int64_t)
LUATYPE(double)

//LUATYPE(string)
typedef struct lua_string {
    std::string v;
    lua_string (std::string v) {
        this->v=v;
    }
    lua_string(){}
}lua_string;

typedef LUATYPE_NAME(double) lua_number;
typedef LUATYPE_NAME(int64_t) lua_int;

enum lua_type_num {
    nlua_int = 0,
    nlua_number = 1,
    nlua_string = 2,
    nlua_bool = 3,
    nlua_table = 4
};

struct SKeyFunctionData {
    bool is_var_arg;                  //是否为可变参数
    std::vector<std::string> arglist; //入参参数名，按序push
};

typedef struct SKeyFunctionData lua_function;

typedef fc::static_variant<
    LUATYPE_NAME(int),    //0
    LUATYPE_NAME(number), //1
    LUATYPE_NAME(string), //2
    LUATYPE_NAME(bool),   //3
    LUATYPE_NAME(table)  //4

    >
    lua_types;

typedef fc::static_variant<
    LUATYPE_NAME(int),
    LUATYPE_NAME(number),
    LUATYPE_NAME(string),
    LUATYPE_NAME(bool)>
    lua_key_variant;

typedef std::map<lua_key, lua_types> lua_map;

typedef struct lua_table {
    lua_map v;
    lua_table(lua_map v) {
        this->v = v;
    }
    lua_table(){};
} lua_table;

typedef struct lua_key {
    lua_key_variant key;
    lua_key(){};
    lua_key(lua_types lkey) {
        key = cast_from_lua_types(lkey);
    }

    static lua_key_variant cast_from_lua_types(lua_types lt) {
        switch (lt.which()) {
        case 0: {
            return lt.get<LUATYPE_NAME(int)>();
        }
        case 1: {
            return lt.get<LUATYPE_NAME(number)>();
        }
        case 2: {
            return lt.get<LUATYPE_NAME(string)>();
        }
        default:
            FC_THROW("lua_key::cast_from_lua_types error ,lua_types:${lua_types}", ("lua_types", lt)); //return LUATYPE_NAME(bool)(false);
        }
    }

    lua_types cast_to_lua_types()const {
        switch (key.which()) {
        case 0: {
            return key.get<LUATYPE_NAME(int)>();
        }
        case 1: {
            return key.get<LUATYPE_NAME(number)>();
        }
        case 2: {
            return key.get<LUATYPE_NAME(string)>();
        }
        default:
            return lua_types();
        }
    }
    lua_key &operator=(const lua_key &v) {
        if (this == &v)
            return *this;
        this->key = v.key;
        return *this;
    }
    lua_key &operator=(const lua_types &lt) {
        key = cast_from_lua_types(lt);
        return *this;
    }

    friend bool operator==(const lua_key &a, const lua_key &b) {
        if (a.key.which() == b.key.which()) {
            switch (a.key.which()) {
            case 0: {
                LUATYPE_NAME(int)
                ad = a.key.get<LUATYPE_NAME(int)>();
                LUATYPE_NAME(int)
                bd = b.key.get<LUATYPE_NAME(int)>();
                return ad.v == bd.v;
            }
            case 1: {
                LUATYPE_NAME(number)
                ad = a.key.get<LUATYPE_NAME(number)>();
                LUATYPE_NAME(number)
                bd = b.key.get<LUATYPE_NAME(number)>();
                return ad.v == bd.v;
            }
            case 2: {
                LUATYPE_NAME(string)
                ad = a.key.get<LUATYPE_NAME(string)>();
                LUATYPE_NAME(string)
                bd = b.key.get<LUATYPE_NAME(string)>();
                return ad.v == bd.v;
            }
            default:
                return false;
            }
        }
        return false;
    }
    friend bool operator<(const lua_key &a, const lua_key &b) {
        if (a.key.which() == b.key.which()) {
            switch (a.key.which()) {
            case 0: {
                LUATYPE_NAME(int)
                ad = a.key.get<LUATYPE_NAME(int)>();
                LUATYPE_NAME(int)
                bd = b.key.get<LUATYPE_NAME(int)>();
                return ad.v < bd.v;
            }
            case 1: {
                LUATYPE_NAME(number)
                ad = a.key.get<LUATYPE_NAME(number)>();
                LUATYPE_NAME(number)
                bd = b.key.get<LUATYPE_NAME(number)>();
                return ad.v < bd.v;
            }
            case 2: {
                LUATYPE_NAME(string)
                ad = a.key.get<LUATYPE_NAME(string)>();
                LUATYPE_NAME(string)
                bd = b.key.get<LUATYPE_NAME(string)>();
                return ad.v < bd.v;
            }
            default:
                return false;
            }
        }
        return a.key.which() < b.key.which();
    }

} lua_key;

} } // namespace bcx::protocol

FC_REFLECT_TYPENAME(bcx::protocol::lua_types)
FC_REFLECT_TYPENAME(bcx::protocol::lua_key_variant)
FC_REFLECT(bcx::protocol::SKeyFunctionData, //(arg_num)(ret_num)
           (is_var_arg)(arglist)              //(retlist)
)
FC_REFLECT(bcx::protocol::LUATYPE_NAME(key), (key))
FC_REFLECT(bcx::protocol::LUATYPE_NAME(bool), (v))
FC_REFLECT(bcx::protocol::LUATYPE_NAME(int64_t), (v))
FC_REFLECT(bcx::protocol::LUATYPE_NAME(string), (v))
FC_REFLECT(bcx::protocol::LUATYPE_NAME(double), (v))
FC_REFLECT(bcx::protocol::LUATYPE_NAME(table), (v))
