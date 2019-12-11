#include "Response.hpp"

namespace bcx {

Response Response::createResponse(int c, const std::string& s) {
    Response resp;
    resp.code = c;
    resp.msg = s;

    return resp;
}

Response Response::createResponse(int c) {
    std::string s = "";
    return createResponse(c, s);
}

Response Response::createResponse(Errors e) {
    int i = (int)e;
    std::string s = "";

    switch (e) {
        case Errors::Error_Success: {
            s = "";
            break;
        }
        case Errors::Error_Error: {
            s = "error";
            break;
        }
        case Errors::Error_Not_Login: {
            s = "not login";
            break;
        }
        case Errors::Error_Auth_Fail: {
            s = "auth failed";
            break;
        }
        case Errors::Error_API_Disable: {
            s = "api disable";
            break;
        }
        case Errors::Error_Chain_Exception: {
            s = "chain exception";
            break;
        }
        case Errors::Error_Chain_Error: {
            s = "chain error";
            break;
        }
        case Errors::Error_Net_Connect: {
            s = "net connect";
            break;
        }
        case Errors::Error_Net_Disconnect: {
            s = "net disconnect";
            break;
        }
        case Errors::Error_Net_Error: {
            s = "net error";
            break;
        }
        default: {
            s = "unknow";
            break;
        }
    }

    return createResponse(i, s);
}

Response Response::createResponse(Errors e, const std::string& s) {
    Response resp = Response::createResponse(e);
    resp.msg = s;

    return resp;
}

Response Response::createResponse(const fc::variant& data) {
    Response resp;
    resp.code = Errors::Error_Success;
    resp.msg = "";
    resp.data = data;

    return resp;
}

}

