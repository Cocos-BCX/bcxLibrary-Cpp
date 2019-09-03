//
//  bcxws_state.cpp
//  bcx
//
//  Created by admin on 2019/3/12.
//  Copyright © 2019 SDKBox. All rights reserved.
//

#include "bcxws.hpp"
#include "bcxlog.hpp"

#include "fc/crypto/elliptic.hpp"
#include "fc/rpc/state.hpp"
#include "fc/io/json.hpp"
#include "fc/rpc/state.hpp"
#include "fc/reflect/variant.hpp"
#include <fc/smart_ref_fwd.hpp>
#include <fc/smart_ref_impl.hpp>
#include <fc/crypto/base58.hpp>

#include <vector>

#include "graphene/chain/protocol/transaction.hpp"
#include "graphene/chain/protocol/asset.hpp"
#include "graphene/chain/protocol/types.hpp"
#include "graphene/chain/protocol/base.hpp"
#include <graphene/chain/protocol/fee_schedule.hpp>
#include <graphene/chain/account_object.hpp>
#include <graphene/chain/balance_object.hpp>
#include <graphene/chain/global_property_object.hpp>

#include "promise.hpp"

using namespace graphene;
using namespace graphene::chain;

NS_BCX_BEGIN

bcxws::bcxws():_on_open(nullptr)
{
}

// ws://echo.websocket.org
// ws://127.0.0.1:8090
// wss://bitshares.openledger.info/ws
// ws://47.93.62.96:8020
bool bcxws::init(const std::string &url, const std::function<void()>& cb)
{
    bool suc = _socket.init(*this, url);
    _on_open = cb;
    return suc;
}

void bcxws::close()
{
    _socket.closeAllConnections();
}

void bcxws::send(const std::string &data)
{
    bcx::log("send> %s", data.c_str());
    if (_socket.getReadyState() != bcx::network::WebSocket::State::OPEN) {
        return ;
    }
    _socket.send(data);
}

::promise::Defer  bcxws::send_call(int api_type, const std::string &api_name, const fc::variant &var,
                      ::promise::Defer defer)
{
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
::promise::Defer  bcxws::send_call(const fc::variants &params, ::promise::Defer defer)
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

::promise::Defer bcxws::broadcast(const fc::variants &params,
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

void bcxws::onOpen(bcx::network::WebSocket* ws) {
    BCXLOG("WS:onOpen");
    _on_open();
}
void bcxws::onMessage(bcx::network::WebSocket* ws, const bcx::network::WebSocket::Data& data) {
    //        dispatch_async(dispatch_get_main_queue(), ^{
    
    if (data.isBinary) {
        bcx::log("WS:onMessage,binary: %ld", data.len);
    } else {
        bcx::log("WS:onMessage,string: %s", data.bytes);
    }
    
    /**
     FIXME:
     交易成功后，返回数据：
     {"method":"notice","params":[11,[{"id":"006300c92230205b30ec39099e17a07a71050197659406898102d4ae4ee77ef5",...]}}]]}
     没有 id
     */
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

void bcxws::onClose(bcx::network::WebSocket* ws) {
    BCXLOG("WS:onClose");
}
void bcxws::onError(bcx::network::WebSocket* ws, const bcx::network::WebSocket::ErrorCode& error) {
    BCXLOG("WS:onError");
}

void bcxws::_oncall(const fc::variant &v)
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
            cbit->second->resolve(v.get_object()["result"]);
            _removeCallback(id);
        } else {
            BCXLOG("WS: ERROR, unhandle variant:%s", fc::json::to_pretty_string(v).c_str());
        }
    } else {
        BCXLOG("WS: callback map not found:%lld", id);
    }
}
void bcxws::_onnotice(const fc::variants &var)
{
//    {"method":"notice","params":[11,[{"id":"006300c92230205b30ec39099e17a07a71050197659406898102d4ae4ee77ef5",...]}}]]}
    
    auto id = var.at(0).as_int64();
    auto cbit = noticeMap.find(id);
    if (cbit == noticeMap.end()) return;

    cbit->second->resolve(var.at(1));
    noticeMap.erase(cbit); // sub ?
    _removeCallback(id);
}

void bcxws::_removeCallback(uint64_t id) {
    callbackMap.erase(id);
    noticeMap.erase(id);
}

bool bcxws::_existInNoticeMap(uint64_t id) {
    if (noticeMap.find(id) != noticeMap.end()) {
        return true;
    }
    return false;
}

NS_BCX_END
