//
//  bcxapi_p.hpp
//  bcx
//
//  Copyright © 2019 SDKBox. All rights reserved.
//

#pragma once

#include <string>
#include <vector>

#include "./bcx_api.hpp"
#include "./bcxws.hpp"
#include "./bcxdb.hpp"
#include "./protocol/contract_object.hpp"
#include "promise.hpp"

NS_BCX_BEGIN

class bcxapi_p
{
public:
    bcxapi_p();
    ~bcxapi_p();
    
    void init(const bcxconfig& cfg, const std::function<void(int)>& cb = nullptr);
    
    void get_full_accounts(const std::string &nameOrId,
                           const std::function<void(graphene::chain::account_object)> &callback = nullptr);

    void createAccount(const std::string &account, const std::string &pw,
                       const std::function<void(bool, const std::string&)> &callback);
    void login(const std::string &account, const std::string &pwd, const std::function<void(int)> &callback);
    void logout();
    bool isLogin();
    void changePassword(const std::string &account, const std::string &oldpw, const std::string &newpw,
                        const std::function<void(bool)> &callback);
    void upgradeAccount(const std::function<void(int)> &callback);
    
    void get_chain_id(const std::function<void(std::string)> &callback);
    void get_objects(const std::vector<std::string> &ids, const std::function<void(const fc::variants&)>& cb);
    void get_key_references(const std::function<void(std::string)> &callback);
    
    void transfer(const std::string& to,
                  const std::string& assetsymbols, int mount,
                  const std::string& payFeeAsset, const std::string& memo,
                  const std::function<void(int, const std::string&)>& cb);
    
    
    /*
     * Asset
     */
    void get_createAsset_fee(const std::function<void(bool)> &callback);
    void lookup_asset_symbols(const std::vector<std::string>& symbols_or_ids,
                              const std::function<void(std::vector<graphene::chain::asset_object>)> &callback);
    void list_assets(const std::string& lower_bound_symbol, int limit,
                     const std::function<void(const std::vector<graphene::chain::asset_object>&, int)> &callback);
    void create_asset(const std::string& symbol,
                      long long maxSupply,
                      int precision,
                      float exchangeRate,
                      const std::string& description,
                      const std::function<void(int)> &callback);
    void update_asset(const std::string& symbol,
                      long long maxSupply,
                      float exchangeRate,
                      const std::string& description,
                      const std::function<void(int)> &callback);
    void issue_asset(const std::string& account,
                     const int mount,
                     const std::string& symbol,
                     const std::string& memo,
                     const std::function<void(int)> &callback);
    void reserve_asset(const std::string& symbol,
                       const int mount,
                       const std::function<void(int)> &callback);

    /*
     * Database
     */
    void get_block_header(unsigned int num, const std::function<void(const fc::optional<graphene::chain::block_header>&)> &cb);
    void get_block(unsigned int num, const std::function<void(const fc::optional<graphene::chain::signed_block>&)> &cb);
    
    /*
     * History
     */
    void get_account_history(const std::string& account,
                             const std::string& stop,
                             unsigned limit,
                             const std::string& start,
                             const std::function<void(const std::vector<graphene::chain::operation_history_object>&)>& cb);
    
    /*
     * Contract
     */
    void create_contract(const std::string& name, const std::string& contract_source,
                         const std::function<void(int)>& callback);
    void get_contract(const std::string& nameOrId,
                      const std::function<void(int error, const bcx::protocol::contract_object&)> &callback);
    void update_contract(const std::string& name_or_id, const std::string& contract_source,
                         const std::function<void(int)>& callback);
    void get_transaction_by_id(const std::string& trx_id,
                               const std::function<void(int, const graphene::chain::signed_transaction&)>& callback);

    void get_account_contract_data(const std::string& accountId, const std::string& contractId);
    void call_contract_function(const std::string& nameOrId,
                                const std::string& functionName,
                                const std::vector<bcx::lua_types>& valueList,
                                int runTime,
                                const std::function<void(int)>& callback);
    
    /*
     * NH Asset
     */
    void nh_register_creator(const std::function<void(int)> &callback);
    void nh_create_world_view(const std::string& name , const std::function<void(int)> &callback);
    void nh_relate_world_view(const std::string& name , const std::function<void(int)> &callback);
    void nh_creat_asset(const std::vector<bcx::nh_asset_create_info>& nh_assets,
                        const std::function<void(int, const std::string&)> &callback);
    void nh_delete_asset(const std::vector<std::string>& ids_or_hashs, const std::function<void(int)> &callback);
    void nh_transfer_asset(const std::string id_or_hash,
                           const std::string to_account,
                           const std::function<void(int)> &callback);
    void nh_lookup_assets(const std::vector<std::string>& ids_or_hashs,
                          const std::function<void(int, const std::vector<fc::optional<bcx::nh_asset_info>>&)> &callback);


    void send(const std::string &data);
    
    void performFunctionInCocosThread(std::function<void ()> function);
    void poolEvent();

private:
    void query_login(const std::function<void(int)> &callback);
    ::promise::Defer query_api_id(const std::string& name, ::promise::Defer defer = promise::newPromise());
    ::promise::Defer get_required_fees(const std::vector<graphene::chain::operation> &ops,
                                       graphene::chain::asset_id_type id,
                                       ::promise::Defer defer = promise::newPromise());
    ::promise::Defer broadcast_transaction_with_callback(const graphene::chain::signed_transaction& trx,
                                                         ::promise::Defer defer = promise::newPromise());
    void query_dynamic_global_property_object();
    ::promise::Defer query_get_full_accounts(const std::string &nameOrId, ::promise::Defer defer = promise::newPromise());
    ::promise::Defer query_get_account_by_name(const std::string &name, ::promise::Defer defer = promise::newPromise());
    ::promise::Defer query_lookup_account_names(const std::vector<std::string> &names, ::promise::Defer defer = promise::newPromise());
    ::promise::Defer query_get_key_references(const std::vector<graphene::chain::public_key_type> &keys, ::promise::Defer defer = promise::newPromise());
    ::promise::Defer send_operation(const std::vector<graphene::chain::operation>& ops,  ::promise::Defer defer = promise::newPromise());
    fc::optional<fc::ecc::private_key> get_current_private_key(const std::string& role);
    fc::optional<fc::ecc::private_key> get_private_key_by_public(const graphene::chain::public_key_type pkeyt);
    ::promise::Defer validate_password(const std::string &account, const std::string &pwd, ::promise::Defer defer = promise::newPromise());
    ::promise::Defer createAccountByAccount(const std::string &account, const std::string &pw,
                       ::promise::Defer defer = promise::newPromise());
    ::promise::Defer createAccountByFaucet(const std::string &account, const std::string &pw,
                       ::promise::Defer defer = promise::newPromise());
    ::promise::Defer lookup_asset_symbols(const std::vector<std::string>& symbols_or_ids,
                                          ::promise::Defer defer = promise::newPromise());
    void comm_fail_hander(::promise::Defer& defer, const std::function<void(int)> &callback);
    ::promise::Defer query_get_transaction_by_id(const std::string& trx_id,
                                            ::promise::Defer defer = promise::newPromise());
    ::promise::Defer query_get_contract(const std::string& nameOrId, ::promise::Defer defer = promise::newPromise());
    ::promise::Defer query_nh_lookup_assets(const std::vector<std::string>& ids_or_hashs,
                                      ::promise::Defer defer = promise::newPromise());
    ::promise::Defer query_get_potential_signatures(const signed_transaction& trx,
                                                    ::promise::Defer defer = promise::newPromise());

private:
    bcxconfig _config;
    bcxws _ws;
    bcxdb _db;
    std::vector<std::function<void()>> _functionsToPerform;
    std::mutex _performMutex;
};

NS_BCX_END
