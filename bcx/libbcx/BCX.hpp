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
    Error_BCX_Error,
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
    static void login(const std::string& account, const std::string& password, const std::function<void(const std::string&)>& cb);
    static void logout();
    static bool isLogin();
    static void getFullAccount(const std::string& nameOrId, const std::function<void(const std::string&)>& cb);
    static void createAccount(const std::string& name, const std::string& pw, const std::function<void(const std::string&)>& cb);
    static void changePassword(const std::string& account,
                               const std::string& oldPW,
                               const std::string& newPW,
                               const std::function<void(const std::string&)>& cb);
    static void upgradeAccount(const std::function<void(const std::string&)>& cb);
    static void getChainId(const std::function<void(const std::string&)>& cb);
    static void getObjects(const std::vector<std::string> &ids, const std::function<void(const std::string&)>& cb);
    static void getKeyReferences(const std::function<void(const std::string&)>& cb);

    /*
     * Asset
     */
    static void transfer(const std::string& toAccount, const std::string& symbol, int mount, const std::string& memo,
                         const std::function<void(const std::string&)>& cb);
    static void lookupAssetSymbols(const std::vector<std::string>& symbolsOrIds,
                                   const std::function<void(const std::string&)>& cb);
    static void listAssets(const std::string& lowerBoundSymbol, int limit,
                           const std::function<void(const std::string&)>& cb);
    static void createAsset(const std::string& symbol,
                            long long maxSupply,
                            int precision,
                            float exchangeRate,
                            const std::string& description,
                            const std::function<void(const std::string&)>& cb);
    static void updateAsset(const std::string& symbol,
                            long long maxSupply,
                            float exchangeRate,
                            const std::string& description,
                            const std::function<void(const std::string&)>& cb);
    static void issueAsset(const std::string& account,
                           const int mount,
                           const std::string& symbol,
                           const std::string& memo,
                           const std::function<void(const std::string&)>& cb);
    static void reserveAsset(const std::string& symbol,
                             const int mount,
                             const std::function<void(const std::string&)>& cb);

    /*
     * Block
     */
    
    /*
     * Utils
     */
    static std::string getVersion();

    /*
     * Loop Event
     */
    static void loop();
};

}
