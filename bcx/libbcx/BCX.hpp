#pragma once

#include <string>
#include <vector>
#include <map>

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
    Error_Not_Own,
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

struct NHAssetCreateInfo {
    std::string assetName;
    std::string worldView;
    std::string baseDescribe;
    std::string owner;
};


typedef int lua_int;
typedef double lua_number;
typedef std::string lua_string;
typedef bool lua_bool;

class lua_value;

typedef std::map<lua_string, lua_value> lua_table;

class lua_value {
public:
    lua_value(): tag(0) { }
    lua_value(const lua_int& i): tag(1) { this->i = i; }
    lua_value(const lua_number& n): tag(2)  { this->n = n; }
    lua_value(const lua_string& str): tag(3) { new (&s) lua_string; s = str; }
    lua_value(const lua_bool& b): tag(4) { this->b = b; }
    lua_value(const lua_table& tab): tag(5) {
        new (&t) lua_table;
        t = tab;
    }
    lua_value(const lua_value& lv) {
        tag = lv.tag;
        switch (lv.tag) {
            case 1: { i = lv.i; break; }
            case 2: { n = lv.n; break; }
            case 3: { new (&s) lua_string; s = lv.s; break; }
            case 4: { b = lv.b; break; }
            case 5: { new (&t) lua_table; t = lv.t; break; }
            default: break;
        }
    }
    ~lua_value() {
        switch (tag) {
            case 3: { s.~lua_string(); break; }
            case 5: { t.~lua_table(); break; }
            default: break;
        }
        tag = 0;
    }

    lua_value& operator=( const lua_value& lv ) {
        if( this == &lv ) return *this;
        tag = lv.tag;
        switch (lv.tag) {
            case 1: { i = lv.i; break; }
            case 2: { n = lv.n; break; }
            case 3: { new (&s) lua_string; s = lv.s; break; }
            case 4: { b = lv.b; break; }
            case 5: { new (&t) lua_table; t = lv.t; break; }
            default: break;
        }
        return *this;
    }
    lua_value& operator=( lua_value&& lv ) {
       if( this == &lv ) return *this;
       tag = lv.tag;
       switch (lv.tag) {
           case 1: { i = lv.i; break; }
           case 2: { n = lv.n; break; }
           case 3: { new (&s) lua_string; s = lv.s; break; }
           case 4: { b = lv.b; break; }
           case 5: { new (&t) lua_table; t = lv.t; break; }
           default: break;
       }
       return *this;
    }

    int tag;
    union {
        lua_int i;
        lua_number n;
        lua_string s;
        lua_bool b;
        lua_table t;
    };
};

typedef std::vector<lua_value> contract_params;

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
    static void getBlockHeader(unsigned int num, const std::function<void(const std::string&)>& cb);
    static void getBlock(unsigned int num, const std::function<void(const std::string&)>& cb);

    /*
     * Contract
     */
    static void createContract(const std::string& name, const std::string& contractSource, const std::function<void(const std::string&)>& cb);
    static void getContract(const std::string& nameOrId, const std::function<void(const std::string&)>& cb);
    static void updateContract(const std::string& nameOrId, const std::string& contractSource, const std::function<void(const std::string&)>& cb);
    static void getTransactionById(const std::string& trxId, const std::function<void(const std::string&)>& cb);
    static void callContractFunction(const std::string& nameOrId, const std::string& functionName,
                                     const contract_params& params, int runTime,
                                     const std::function<void(const std::string&)>& cb);

    /*
     * NH Asset
     */
    static void NHRegisterCreator(const std::function<void(const std::string&)>& cb);
    static void NHCreateWorldView(const std::string& name, const std::function<void(const std::string&)>& cb);
    static void NHRelateWorldView(const std::string& name, const std::function<void(const std::string&)>& cb);
    static void NHCreatAsset(const std::vector<bcx::NHAssetCreateInfo>& NHAssets, const std::function<void(const std::string&)>& cb);
    static void NHDeleteAsset(const std::vector<std::string>& IDOrHashs, const std::function<void(const std::string&)>& cb);
    static void NHTransferAsset(const std::string IDOrHash, const std::string ToAccount, const std::function<void(const std::string&)>& cb);
    static void NHLookupAssets(const std::vector<std::string>& IDOrHashs, const std::function<void(const std::string&)>& cb);

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
