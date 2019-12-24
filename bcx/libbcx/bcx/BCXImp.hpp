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
    
    BCXImp();

    void init(const Config& cfg);
    void connect(const std::function<void(const std::string&)>& connectStatus);
    void disconnect();
    void login(const std::string& account, const std::string& password, const std::function<void(const std::string&)>& cb);
    void logout();
    bool isLogin();
    void getFullAccount(const std::string& nameOrId, const std::function<void(const std::string&)>& cb);
    void createAccount(const std::string& name, const std::string& pw, const std::function<void(const std::string&)>& cb);
    void changePassword(const std::string& account,
                        const std::string& oldPW,
                        const std::string& newPW,
                        const std::function<void(const std::string&)>& cb);
    void upgradeAccount(const std::function<void(const std::string&)>& cb);
    void getChainId(const std::function<void(const std::string&)>& cb);
    void getObjects(const std::vector<std::string> &ids, const std::function<void(const std::string&)>& cb);
    void getKeyReferences(const std::function<void(const std::string&)>& cb);

    void transfer(const std::string& toAccount, const std::string& symbol, int mount, const std::string& memo,
                  const std::function<void(const std::string&)>& cb);

    void performFunctionInMainThread(const std::function<void()>& f);
    void loop();

private:

    ::promise::Defer enableChainAPI();
    ::promise::Defer getFullAccounts(const std::string &nameOrId);
    ::promise::Defer getObjects(const std::vector<std::string> &ids);
    ::promise::Defer lookupAssetSymbols(const std::vector<std::string>& symbols_or_ids);
    ::promise::Defer sendOperation(const std::vector<bcx::protocol::operation>& ops, const fc::optional<fc::ecc::private_key>& oSignKey = fc::optional<fc::ecc::private_key>());
    ::promise::Defer broadcastTransactionWithCallback(const bcx::protocol::signed_transaction& trx);
    ::promise::Defer createAccountByAccount(const std::string &account, const std::string &pw);
    ::promise::Defer createAccountByFaucet(const std::string &account, const std::string &pw);
    fc::optional<fc::ecc::private_key> getCurrentPrivateKey(const std::string& role);
    void resetChainData();
    bool isChainAPIOpen();

private:
    static BCXImp* gInstance;

    Config _cfg;
    BCXWS _ws;
    ChainData _chainData;
    Looper _looper;
};

}
