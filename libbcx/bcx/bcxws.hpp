//
//  bcxws_state.hpp
//  bcx
//
//  Created by admin on 2019/3/12.
//  Copyright © 2019 SDKBox. All rights reserved.
//

#ifndef bcxws_hpp
#define bcxws_hpp

#include "platform/PlatformMacros.h"
#include "network/WebSocket.hpp"

#include "fc/rpc/state.hpp"
#include "fc/io/json.hpp"
#include "fc/reflect/variant.hpp"

#include "promise.hpp"

NS_BCX_BEGIN

class bcxws : public bcx::network::WebSocket::Delegate
{
public:
    bcxws();
    bool init(const std::string& url, const std::function<void()>& cb = nullptr);
    void close();
    
    void send(const std::string& data);
    
    ::promise::Defer send_call(int api_type, const std::string &api_name, const fc::variant &var,
                               ::promise::Defer defer = ::promise::newPromise());
    ::promise::Defer send_call(const fc::variants &params,
                               ::promise::Defer defer = ::promise::newPromise());
    ::promise::Defer broadcast(const fc::variants &params,
                               ::promise::Defer defer = ::promise::newPromise());
private:
    void onOpen(bcx::network::WebSocket* ws) override;
    void onMessage(bcx::network::WebSocket* ws, const bcx::network::WebSocket::Data& data) override;
    void onClose(bcx::network::WebSocket* ws) override;
    void onError(bcx::network::WebSocket* ws, const bcx::network::WebSocket::ErrorCode& error) override;
    
    void _oncall(const fc::variant &var);
    void _onnotice(const fc::variants &var);

    void _removeCallback(uint64_t id);
    bool _existInNoticeMap(uint64_t id);

    std::unordered_map< uint64_t, ::promise::Defer > callbackMap;
    std::unordered_map< uint64_t, ::promise::Defer > noticeMap;
    
    uint64_t _counter = 0;
    bcx::network::WebSocket _socket;
    std::function<void()> _on_open;
};

NS_BCX_END

#endif /* bcxws_hpp */
