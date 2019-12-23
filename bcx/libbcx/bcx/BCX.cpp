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

std::string BCX::getVersion() {
    return BCX_SDK_VERSION_STR;
}

void BCX::loop() {
    BCXImp::getInstance()->loop();
}

}
