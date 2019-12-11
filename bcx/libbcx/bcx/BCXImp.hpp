#pragma once

#include "../BCX.hpp"
#include "../network/BCXWS.hpp"
#include "../fc/include/fc/crypto/elliptic.hpp"
#include "./Looper.hpp"
#include "../protocol/dynamic_global_property_object.hpp"
#include "../protocol/full_account.hpp"
#include "../protocol/operations/operations.hpp"
#include "../protocol/transaction.hpp"

namespace bcx {

struct ChainData {
    int dbAPIID = -1;
    int networkAPIID = -1;
    int historyAPIID = -1;
    std::string chainID = "";

    std::map<std::string, fc::ecc::private_key> savedKeys;
    bcx::protocol::dynamic_global_property_object dgpo;
    bcx::protocol::full_account fullAccount;
};

class BCXImp {

public:
    
    static BCXImp* getInstance();

    void init(const Config& cfg);
    void connect(const std::function<void(const std::string&)>& connectStatus);
    void disconnect();
    void login(const std::string& account, const std::string& password, const std::function<void(std::string&)>& cb);
    void transfer(const std::string& toAccount, const std::string& symbol, int mount, const std::string& memo,
                  const std::function<void(std::string&)>& cb);

    void performFunctionInMainThread(const std::function<void()>& f);
    void loop();

private:

    ::promise::Defer enableChainAPI();
    ::promise::Defer getFullAccounts(const std::string &nameOrId);
    ::promise::Defer getObjects(const std::vector<std::string> &ids);
    ::promise::Defer lookupAssetSymbols(const std::vector<std::string>& symbols_or_ids);
    ::promise::Defer sendOperation(const std::vector<bcx::protocol::operation>& ops);
    ::promise::Defer broadcastTransactionWithCallback(const bcx::protocol::signed_transaction& trx);
    fc::optional<fc::ecc::private_key> getCurrentPrivateKey(const std::string& role);
    void resetChainData();
    bool isChainAPIOpen();
    bool isLogin();

private:
    static BCXImp* gInstance;

    Config _cfg;
    BCXWS _ws;
    ChainData _chainData;
    Looper _looper;
};

}
