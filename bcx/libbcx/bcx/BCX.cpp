#include "../BCX.hpp"
#include "./BCXImp.hpp"

namespace bcx {

void BCX::init(const Config& cfg) {
    BCXImp::getInstance()->init(cfg);
}

void BCX::connect(const std::function<void(const std::string&)>& connectStatus) {
    BCXImp::getInstance()->connect(connectStatus);
}

void BCX::disconnect() {
}

void BCX::login(const std::string& account, const std::string& password, const std::function<void(std::string&)>& cb) {
    BCXImp::getInstance()->login(account, password, cb);
}

void BCX::transfer(const std::string& toAccount, const std::string& symbol, int mount, const std::string& memo,
                   const std::function<void(std::string&)>& cb) {
    BCXImp::getInstance()->transfer(toAccount, symbol, mount, memo, cb);
}

void BCX::loop() {
    BCXImp::getInstance()->loop();
}

}
