#pragma once

#include <string>
#include <vector>

#include "./bcxconfig.hpp"
#include "./nh_asset_info.hpp"
#include "./protocol/signed_block.hpp"
#include "graphene/chain/protocol/base.hpp"
#include "graphene/chain/protocol/block.hpp"

NS_BCX_BEGIN

struct bcxapi_p;
struct bcxapi
{
    static bcxapi & instance();
    
    std::string getVersion();

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

    /*
     * Database
     */
    void get_block_header(unsigned int num, const std::function<void(const fc::optional<graphene::chain::block_header>&)> &cb);
    void get_block(unsigned int num, const std::function<void(const fc::optional<bcx::protocol::signed_block>&)> &cb);

    /*
     * History
     */
    void get_account_history(const std::string& account,
                             const std::string& stop,
                             unsigned limit,
                             const std::string& start,
                             const std::function<void(const std::vector<graphene::chain::operation_history_object>&)>& cb);
    
    void create_contract(const std::string& name, const std::string& contract_source,
                         const std::function<void(int)>& callback);
    void get_contract(const std::string& nameOrId,
                      const std::function<void(int error, const bcx::protocol::contract_object&)> &callback);
    void update_contract(const std::string& name_or_id, const std::string& contract_source,
                         const std::function<void(int)>& callback);
    void get_transaction_by_id(const std::string& trx_id,
                               const std::function<void(int, const graphene::chain::signed_transaction&)>& callback);

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
    void nh_creat_asset(const std::vector<bcx::nh_asset_create_info>& nh_assets, const std::function<void(int, const std::string&)> &callback);
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
    bcxapi();
    ~bcxapi();

private:
    std::shared_ptr<bcxapi_p> my;
};

NS_BCX_END
