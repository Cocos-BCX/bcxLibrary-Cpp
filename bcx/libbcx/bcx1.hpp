#pragma once

#include <string>

namespace bcx {

struct Config {
    std::string wsNode;
    std::string faucetURL;
    bool autoConnect;
};

enum Errors {
    Error_Success,
    Error_Error,
    Error_Unknow,
    Error_Not_Login,
    Error_Auth_Fail,
    Error_API_Disable,
    Error_Chain_Exception,
    Error_Chain_Error,
    Error_Net_Connect,
    Error_Net_Disconnect,
    Error_Net_Error,
    Error_Max
};

class BCX {

public:
    static void init(const Config& cfg);
    static void connect(const std::function<void(const std::string&)>& connectStatus);
    static void disconnect();
    
    /*
     * Account
     */
    static void login(const std::string& account, const std::string& password, const std::function<void(std::string&)>& cb);

    /*
     * Asset
     */
    static void transfer(const std::string& toAccount, const std::string& symbol, int mount, const std::string& memo,
                         const std::function<void(std::string&)>& cb);

    /*
     * Loop Event
     */
    static void loop();
};

}
