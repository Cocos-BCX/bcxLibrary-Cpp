#include <vector>

#include "BCXWS.hpp"
#include "../log/BCXLog.hpp"
#include "../bcx/Timer.hpp"

#include <fc/rpc/state.hpp>
#include <fc/io/json.hpp>


namespace bcx {

BCXWS::BCXWS():_onNetStatus(nullptr){
}

bool BCXWS::init(const std::string &url,
                 const std::function<void(const Response& resp)>& cb) {
    _url = url;
    bool suc = _socket.init(*this, url);
    _onNetStatus = cb;

    setAutoConnect(true);

    return suc;
}

void BCXWS::setAutoConnect(bool b) {
    _autoConnect = b;
}

void BCXWS::close() {
    _socket.closeAllConnections();
}

void BCXWS::send(const std::string &data) {
    BCXLOG("send> %s", data.c_str());
    if (_socket.getReadyState() != bcx::network::WebSocket::State::OPEN) {
        bcx::log("ERROR! socket is not open.");
        return;
    }
    _socket.send(data);
}

::promise::Defer  BCXWS::send_call(int api_type, const std::string &api_name, const fc::variant &var,
                      ::promise::Defer defer) {
    if (_socket.getReadyState() != bcx::network::WebSocket::State::OPEN) {
        BCXLOG("ERROR! socket is not open.");
        TimerMgr::instance().addTimer([defer](int i) {
            defer.reject(Response::createResponse(Errors::Error_Net_Disconnect));
        }, 10, false);
        return defer;
    }

    fc::variants params;
    params.push_back(api_type);
    params.push_back(api_name);
    params.push_back(var);
    
    if (api_type == 3) {
        broadcast(params, defer);
    } else {
        send_call(params, defer);
    }
    
    return defer;
}
::promise::Defer  BCXWS::send_call(const fc::variants &params, ::promise::Defer defer)
{
    ++_counter;
    callbackMap[_counter] = defer;
    
    fc::rpc::request request{_counter, "call", params};
    std::string s = fc::json::to_string(fc::variant(request, (uint32_t)100),
                                        fc::json::stringify_large_ints_and_doubles,
                                        100);
    send(s);
    return defer;
}

::promise::Defer BCXWS::broadcast(const fc::variants &params,
                                  ::promise::Defer defer)
{
    ++_counter;
    callbackMap[_counter] = defer;
    noticeMap[_counter] = defer;

    auto newParams = params;
    auto &arr = newParams.at(2).get_array();
    arr.insert(arr.begin(), fc::variant(_counter));
    
    fc::rpc::request request{_counter, "call", newParams};
    std::string s = fc::json::to_string(fc::variant(request, (uint32_t)100),
                                        fc::json::stringify_large_ints_and_doubles,
                                        100);
    send(s);
    return defer;
}

// private

void BCXWS::onOpen(bcx::network::WebSocket* ws) {
    BCXLOG("WS:onOpen");
    if (_onNetStatus) {
        _onNetStatus(Response::createResponse(Errors::Error_Net_Connect));
    }
}
void BCXWS::onMessage(bcx::network::WebSocket* ws, const bcx::network::WebSocket::Data& data) {
    if (data.isBinary) {
        BCXLOG("WS:onMessage,binary: %ld", data.len);
    } else {
        BCXLOG("WS:onMessage,string: %s", data.bytes);
    }

    fc::variant v = fc::json::from_string(data.bytes);
    auto &vv = v.get_object();
    if (vv.contains("id")) {
        _oncall(v);
    } else if (vv.contains("method")) {
        if (vv["method"].as_string() == "notice") {
            _onnotice(vv["params"].get_array());
        }
    }
}

void BCXWS::onClose(bcx::network::WebSocket* ws) {
    BCXLOG("WS:onClose");
    _rejectAllRqeuest(Response::createResponse(Errors::Error_Net_Disconnect));
    if (_onNetStatus) {
        _onNetStatus(Response::createResponse(Errors::Error_Net_Disconnect));
    }
    if (_autoConnect) {
        TimerMgr::instance().addTimer([this](int i) {
            this->_socket.init(*this, this->_url);
        }, 1000 * 10, false);
    }
}

void BCXWS::onError(bcx::network::WebSocket* ws, const bcx::network::WebSocket::ErrorCode& error) {
    BCXLOG("WS:onError");
    _rejectAllRqeuest(Response::createResponse(Errors::Error_Net_Error));
    if (_onNetStatus) {
        _onNetStatus(Response::createResponse(Errors::Error_Net_Error));
        _onNetStatus = nullptr;
    }
}

void BCXWS::_rejectAllRqeuest(Response resp) {
    for (const auto& it : callbackMap) {
        it.second.reject(resp);
        noticeMap.erase(it.first);
    }
    for (const auto& it : noticeMap) {
        it.second.reject(resp);
    }
    callbackMap.clear();
    noticeMap.clear();
}

void BCXWS::_oncall(const fc::variant &v)
{
    auto id = v.get_object()["id"].as_int64();
    
    auto cbit = callbackMap.find(id);
    if (cbit != callbackMap.end()) {
        if (v.get_object().contains("error")) {
            fc::rpc::error_object err;
            v.get_object()["error"].as(err, BCX_PACK_MAX_DEPTH);
            cbit->second->reject(err);
            _removeCallback(id);
        } else if (v.get_object().contains("result")) {
            const auto& rst = v.get_object()["result"];
            if (_existInNoticeMap(id)) {
                if (rst.is_null()) {
                    return;
                }
            }
            cbit->second->resolve(rst);
            _removeCallback(id);
        } else {
            BCXLOG("WS: ERROR, unhandle variant:%s", fc::json::to_pretty_string(v).c_str());
        }
    } else {
        BCXLOG("WS: callback map not found:%lld", id);
    }
}
void BCXWS::_onnotice(const fc::variants &var)
{
//    {"method":"notice","params":[11,[{"id":"006300c92230205b30ec39099e17a07a71050197659406898102d4ae4ee77ef5",...]}}]]}
    
    auto id = var.at(0).as_int64();
    auto cbit = noticeMap.find(id);
    if (cbit == noticeMap.end()) return;

    cbit->second->resolve(var.at(1));
    noticeMap.erase(cbit); // sub ?
    _removeCallback(id);
}

void BCXWS::_removeCallback(uint64_t id) {
    callbackMap.erase(id);
    noticeMap.erase(id);
}

bool BCXWS::_existInNoticeMap(uint64_t id) {
    if (noticeMap.find(id) != noticeMap.end()) {
        return true;
    }
    return false;
}

}

