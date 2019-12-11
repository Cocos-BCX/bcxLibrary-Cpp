#pragma once

#include "../platform/PlatformMacros.h"
#include "./WebSocket.hpp"
#include "../3rd/promise-cpp/promise.hpp"
#include "../fc/include/fc/reflect/variant.hpp"
#include "../bcx/Response.hpp"

namespace bcx {

class BCXWS : public bcx::network::WebSocket::Delegate {
public:
    BCXWS();

    bool init(const std::string& url, const std::function<void(const Response& resp)>& cb = nullptr);
    void setAutoConnect(bool b);
    void close();

    void send(const std::string& data);
    
    ::promise::Defer send_call(int api_type, const std::string &api_name, const fc::variant &var,
                               promise::Defer defer = promise::newPromise());
    ::promise::Defer send_call(const fc::variants &params,
                               ::promise::Defer defer = promise::newPromise());
    ::promise::Defer broadcast(const fc::variants &params,
                               ::promise::Defer defer = promise::newPromise());
private:
    void onOpen(bcx::network::WebSocket* ws) override;
    void onMessage(bcx::network::WebSocket* ws, const bcx::network::WebSocket::Data& data) override;
    void onClose(bcx::network::WebSocket* ws) override;
    void onError(bcx::network::WebSocket* ws, const bcx::network::WebSocket::ErrorCode& error) override;

    void _rejectAllRqeuest(Response resp);

    void _oncall(const fc::variant &var);
    void _onnotice(const fc::variants &var);

    void _removeCallback(uint64_t id);
    bool _existInNoticeMap(uint64_t id);

    std::unordered_map< uint64_t, ::promise::Defer > callbackMap;
    std::unordered_map< uint64_t, ::promise::Defer > noticeMap;
    
    uint64_t _counter = 0;
    std::string _url;
    bool _autoConnect;

    bcx::network::WebSocket _socket;
    std::function<void(const Response& resp)> _onNetStatus;
};

}

