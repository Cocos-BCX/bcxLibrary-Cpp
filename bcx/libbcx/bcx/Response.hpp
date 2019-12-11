#pragma once

#include "../BCX.hpp"
#include "../fc/include/fc/reflect/reflect.hpp"
#include "../fc/include/fc/variant.hpp"

namespace bcx {

struct Response {
    int code;
    std::string msg;
    fc::variant data;

    static Response createResponse(int c, const std::string& s);
    static Response createResponse(int c);
    static Response createResponse(Errors e);
    static Response createResponse(Errors e, const std::string& s);
    static Response createResponse(const fc::variant& data);
};

}

FC_REFLECT(bcx::Response,
 (code)
 (msg)
)

