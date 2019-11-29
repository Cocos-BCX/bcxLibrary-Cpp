//
//  bcx_api.cpp
//  bcx
//
//  Created by admin on 2019/3/12.
//  Copyright © 2019 SDKBox. All rights reserved.
//

#include <vector>
#include <boost/algorithm/string/predicate.hpp>

#include "bcx_api.hpp"
#include "bcxlog.hpp"
#include "bcxtimer.hpp"
#include "./protocol/full_account.hpp"

#include "promise.hpp"

#include "bcxapi_p.hpp"

using namespace graphene;
using namespace graphene::chain;

#define BCX_SDK_VERSION_STR "1.1.0"

NS_BCX_BEGIN

bcxapi::bcxapi() {
    my = std::make_shared<bcxapi_p>();
}
bcxapi::~bcxapi() {
}

bcxapi & bcxapi::instance() {
    static bcxapi self;
    return self;
}

std::string bcxapi::getVersion() {
    return BCX_SDK_VERSION_STR;
}

void bcxapi::init(const bcxconfig& cfg, const std::function<void(int)>& cb) {
    my->init(cfg, cb);
}

void bcxapi::createAccount(const std::string &account, const std::string &pw,
                   const std::function<void(bool, const std::string&)> &callback) {
    my->createAccount(account, pw, callback);
}

void bcxapi::login(const std::string &account, const std::string &pwd, const std::function<void(int)>&callback) {
    my->login(account, pwd, callback);
}

void bcxapi::logout() {
    my->logout();
}

bool bcxapi::isLogin() {
    return my->isLogin();
}

void bcxapi::changePassword(const std::string &account,
                    const std::string &oldpw,
                    const std::string &newpw,
                    const std::function<void(bool)> &callback) {
    my->changePassword(account, oldpw, newpw, callback);
}

void bcxapi::upgradeAccount(const std::function<void(int)> &callback) {
    my->upgradeAccount(callback);
}

void bcxapi::get_full_accounts(const std::string &nameOrId,
                                const std::function<void(graphene::chain::account_object)> &callback)
{
    my->get_full_accounts(nameOrId, callback);
}

void bcxapi::get_chain_id(const std::function<void(std::string)> &callback)
{
    my->get_chain_id(callback);
}

void bcxapi::get_objects(const std::vector<std::string> &ids, const std::function<void(const fc::variants&)>& cb)
{
    my->get_objects(ids, cb);
}

void bcxapi::get_key_references(const std::function<void(std::string)> &callback)
{
    my->get_key_references(callback);
}

void bcxapi::send(const std::string& data)
{
    my->send(data);
}

void bcxapi::performFunctionInCocosThread(std::function<void ()> function)
{
    my->performFunctionInCocosThread(function);
}

void bcxapi::poolEvent()
{
    my->poolEvent();
}

void bcxapi::transfer(const std::string& to,
                      const std::string& assetsymbols, int mount,
                      const std::string& payFeeAsset, const std::string& memo_,
                      const std::function<void(int, const std::string&)>& cb) {
    my->transfer(to, assetsymbols, mount, payFeeAsset, memo_, cb);
}

void bcxapi::get_createAsset_fee(const std::function<void(bool)> &callback) {
    my->get_createAsset_fee(callback);
}

void bcxapi::lookup_asset_symbols(const std::vector<std::string>& symbols_or_ids,
                          const std::function<void(std::vector<graphene::chain::asset_object>)> &callback) {
    my->lookup_asset_symbols(symbols_or_ids, callback);
}

void bcxapi::list_assets(const std::string& lower_bound_symbol, int limit,
                 const std::function<void(const std::vector<graphene::chain::asset_object>&, int)> &callback) {
    my->list_assets(lower_bound_symbol, limit, callback);
}

void bcxapi::create_asset(const std::string& symbol,
                  long long maxSupply,
                  int precision,
                  float exchangeRate,
                  const std::string& description,
                  const std::function<void(int)> &callback) {
    my->create_asset(symbol, maxSupply, precision, exchangeRate, description, callback);
}

void bcxapi::update_asset(const std::string& symbol,
                  long long maxSupply,
                  float exchangeRate,
                  const std::string& description,
                  const std::function<void(int)> &callback) {
    my->update_asset(symbol, maxSupply, exchangeRate, description, callback);
}

void bcxapi::issue_asset(const std::string& account,
                 const int mount,
                 const std::string& symbol,
                 const std::string& memo,
                 const std::function<void(int)> &callback) {
    my->issue_asset(account, mount, symbol, memo, callback);
}

void bcxapi::get_block_header(unsigned int num, const std::function<void(const fc::optional<graphene::chain::block_header>&)> &cb) {
    my->get_block_header(num, cb);
}

void bcxapi::get_block(unsigned int num, const std::function<void(const fc::optional<bcx::protocol::signed_block>&)> &cb) {
    my->get_block(num, cb);
}

void bcxapi::get_account_history(const std::string& account,
                                 const std::string& stop,
                                 unsigned limit,
                                 const std::string& start,
                                 const std::function<void(const std::vector<graphene::chain::operation_history_object>&)>& cb) {
    my->get_account_history(account, stop, limit, start, cb);
}

void bcxapi::create_contract(const std::string& name, const std::string& contract_source,
                             const std::function<void(int)>& callback) {
    my->create_contract(name, contract_source, callback);
}

void bcxapi::get_contract(const std::string& nameOrId,
                          const std::function<void(int, const bcx::protocol::contract_object&)> &callback) {
    my->get_contract(nameOrId, callback);
}

void bcxapi::update_contract(const std::string& name_or_id, const std::string& contract_source,
                     const std::function<void(int)>& callback) {
    my->update_contract(name_or_id, contract_source, callback);
}

void bcxapi::get_transaction_by_id(const std::string& trx_id,
                                   const std::function<void(int, const graphene::chain::signed_transaction&)>& callback) {
    my->get_transaction_by_id(trx_id, callback);
}

void bcxapi::call_contract_function(const std::string& nameOrId,
                            const std::string& functionName,
                            const std::vector<bcx::lua_types>& valueList,
                            int runTime,
                            const std::function<void(int)>& callback) {
    my->call_contract_function(nameOrId, functionName, valueList, runTime, callback);
}

void bcxapi::nh_register_creator(const std::function<void(int)> &callback) {
    my->nh_register_creator(callback);
}

void bcxapi::nh_create_world_view(const std::string& name , const std::function<void(int)> &callback) {
    my->nh_create_world_view(name, callback);
}

void bcxapi::nh_relate_world_view(const std::string& name , const std::function<void(int)> &callback) {
    my->nh_relate_world_view(name, callback);
}

void bcxapi::nh_creat_asset(const std::vector<bcx::nh_asset_create_info>& nh_assets,
                            const std::function<void(int, const std::string&)> &callback) {
    my->nh_creat_asset(nh_assets, callback);
}

void bcxapi::nh_delete_asset(const std::vector<std::string>& ids_or_hashs, const std::function<void(int)> &callback) {
    my->nh_delete_asset(ids_or_hashs, callback);
}

void bcxapi::nh_transfer_asset(const std::string id_or_hash,
                       const std::string to_account,
                       const std::function<void(int)> &callback) {
    my->nh_transfer_asset(id_or_hash, to_account, callback);
}

void bcxapi::nh_lookup_assets(const std::vector<std::string>& ids_or_hashs,
                      const std::function<void(int, const std::vector<fc::optional<bcx::nh_asset_info>>&)> &callback) {
    my->nh_lookup_assets(ids_or_hashs, callback);
}

NS_BCX_END
