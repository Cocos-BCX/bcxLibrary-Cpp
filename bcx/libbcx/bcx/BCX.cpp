#include "../BCX.hpp"
#include "./BCXImp.hpp"

#define BCX_SDK_VERSION_STR "1.1.0"

namespace bcx {

void BCX::init(const Config& cfg) {
    BCXImp::getInstance()->init(cfg);
}

void BCX::connect(const std::function<void(const std::string&)>& connectStatus) {
    BCXImp::getInstance()->connect(connectStatus);
}

void BCX::disconnect() {
}

void BCX::login(const std::string& account, const std::string& password, const std::function<void(const std::string&)>& cb) {
    BCXImp::getInstance()->login(account, password, cb);
}

void BCX::logout() {
    BCXImp::getInstance()->logout();
}

bool BCX::isLogin() {
    return BCXImp::getInstance()->isLogin();
}

void BCX::getFullAccount(const std::string& nameOrId, const std::function<void(const std::string&)>& cb) {
    BCXImp::getInstance()->getFullAccount(nameOrId, cb);
}

void BCX::createAccount(const std::string& name, const std::string& pw, const std::function<void(const std::string&)>& cb) {
    BCXImp::getInstance()->createAccount(name, pw, cb);
}

void BCX::changePassword(const std::string& account,
                         const std::string& oldPW,
                         const std::string& newPW,
                         const std::function<void(const std::string&)> &cb) {
    BCXImp::getInstance()->changePassword(account, oldPW, newPW, cb);
}

void BCX::upgradeAccount(const std::function<void(const std::string&)>& cb) {
    BCXImp::getInstance()->upgradeAccount(cb);
}

void BCX::getChainId(const std::function<void(const std::string&)>& cb) {
    BCXImp::getInstance()->getChainId(cb);
}

void BCX::getObjects(const std::vector<std::string> &ids, const std::function<void(const std::string&)>& cb) {
    BCXImp::getInstance()->getObjects(ids, cb);
}

void BCX::getKeyReferences(const std::function<void(const std::string&)>& cb) {
    BCXImp::getInstance()->getKeyReferences(cb);
}

void BCX::transfer(const std::string& toAccount, const std::string& symbol, int mount, const std::string& memo,
                   const std::function<void(const std::string&)>& cb) {
    BCXImp::getInstance()->transfer(toAccount, symbol, mount, memo, cb);
}

void BCX::lookupAssetSymbols(const std::vector<std::string>& symbolsOrIds,
                            const std::function<void(const std::string&)>& cb) {
    BCXImp::getInstance()->lookupAssetSymbols(symbolsOrIds, cb);
}

//void BCX::listAssets(const std::string& lowerBoundSymbol, int limit,
//                    const std::function<void(const std::string&)>& cb) {
//    BCXImp::getInstance()->listAssets(lowerBoundSymbol, limit, cb);
//}

void BCX::createAsset(const std::string& symbol,
                    long long maxSupply,
                    int precision,
                    float exchangeRate,
                    const std::string& description,
                    const std::function<void(const std::string&)>& cb) {
    BCXImp::getInstance()->createAsset(symbol, maxSupply, precision, exchangeRate, description, cb);
}

void BCX::updateAsset(const std::string& symbol,
                    long long maxSupply,
                    float exchangeRate,
                    const std::string& description,
                    const std::function<void(const std::string&)>& cb) {
    BCXImp::getInstance()->updateAsset(symbol, maxSupply, exchangeRate, description, cb);
}

void BCX::issueAsset(const std::string& account,
                    const int mount,
                    const std::string& symbol,
                    const std::string& memo,
                    const std::function<void(const std::string&)>& cb) {
    BCXImp::getInstance()->issueAsset(account, mount, symbol, memo, cb);
}

void BCX::reserveAsset(const std::string& symbol,
                    const int mount,
                    const std::function<void(const std::string&)>& cb) {
    BCXImp::getInstance()->reserveAsset(symbol, mount, cb);
}

void BCX::getBlockHeader(unsigned int num, const std::function<void(const std::string&)>& cb) {
    BCXImp::getInstance()->getBlockHeader(num, cb);
}

void BCX::getBlock(unsigned int num, const std::function<void(const std::string&)>& cb) {
    BCXImp::getInstance()->getBlock(num, cb);
}

void BCX::createContract(const std::string& name, const std::string& contractSource, const std::function<void(const std::string&)>& cb) {
    BCXImp::getInstance()->createContract(name, contractSource, cb);
}

void BCX::getContract(const std::string& nameOrId, const std::function<void(const std::string&)>& cb) {
    BCXImp::getInstance()->getContract(nameOrId, cb);
}

void BCX::updateContract(const std::string& nameOrId, const std::string& contractSource, const std::function<void(const std::string&)>& cb) {
    BCXImp::getInstance()->updateContract(nameOrId, contractSource, cb);
}

void BCX::getTransactionById(const std::string& trxId, const std::function<void(const std::string&)>& cb) {
    BCXImp::getInstance()->getTransactionById(trxId, cb);
}

void BCX::callContractFunction(const std::string& nameOrId, const std::string& functionName,
                               const contract_params& params, int runTime,
                               const std::function<void(const std::string&)>& cb) {
    BCXImp::getInstance()->callContractFunction(nameOrId, functionName, params, runTime, cb);
}


std::string BCX::getVersion() {
    return BCX_SDK_VERSION_STR;
}

void BCX::loop() {
    BCXImp::getInstance()->loop();
}

}
