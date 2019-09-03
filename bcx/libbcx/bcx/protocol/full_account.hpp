#pragma once

#include <graphene/chain/account_object.hpp>
#include <graphene/chain/vesting_balance_object.hpp>
#include <graphene/chain/market_evaluator.hpp>
#include <graphene/chain/withdraw_permission_object.hpp>

namespace bcx { namespace protocol {
    using namespace graphene::chain;

    struct full_account {
        account_object                          account;
        account_statistics_object               statistics;
        string                                  registrar_name;
        string                                  referrer_name;
        string                                  lifetime_referrer_name;
        std::vector<variant>                    votes;
        optional<vesting_balance_object>        cashback_balance;
        std::vector<account_balance_object>     balances;
        std::vector<vesting_balance_object>     vesting_balances;
        std::vector<limit_order_object>         limit_orders;
        std::vector<call_order_object>          call_orders;
        std::vector<force_settlement_object>    settle_orders;
        std::vector<proposal_object>            proposals;
        std::vector<asset_id_type>              assets;
        std::vector<withdraw_permission_object> withdraws;
    };

} }

FC_REFLECT( bcx::protocol::full_account,
            (account)
            (statistics)
            (registrar_name)
            (referrer_name)
            (lifetime_referrer_name)
            (votes)
            (cashback_balance)
            (balances)
            (vesting_balances)
            (limit_orders)
            (call_orders)
            (settle_orders)
            (assets)
            (withdraws)
           )
