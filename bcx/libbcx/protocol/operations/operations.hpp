#pragma once

#include "../../fc/include/fc/static_variant.hpp"
#include "./account.hpp"
#include "./contract.hpp"
#include "../asset_ops.hpp"
#include "./market.hpp"
#include "./placehoder.hpp"
#include "./transfer.hpp"


namespace bcx {

namespace protocol {

typedef fc::static_variant<
    transfer_operation,
    limit_order_create_operation,
    limit_order_cancel_operation,
    call_order_update_operation,
    fill_order_operation,
    account_create_operation,
    account_update_operation,
    account_upgrade_operation,
    asset_create_operation,
    asset_update_operation,
    asset_update_restricted,
    asset_update_bitasset_operation,
    asset_update_feed_producers_operation,
    asset_issue_operation,
    asset_reserve_operation,
    asset_settle_operation,
    asset_global_settle_operation,
    asset_publish_feed_operation,
    witness_create_operation,
    witness_update_operation,
    proposal_create_operation,
    proposal_update_operation,
    proposal_delete_operation,
    committee_member_create_operation,
    committee_member_update_operation,
    committee_member_update_global_parameters_operation,
    vesting_balance_create_operation,
    vesting_balance_withdraw_operation,
    worker_create_operation,
    balance_claim_operation,
    asset_settle_cancel_operation,
    asset_claim_fees_operation,
    bid_collateral_operation,
    execute_bid_operation,
    contract_create_operation,
    contract_call_function_operation,
    temporary_authority_chang,
    nh_register_asset_creator_operation,
    nh_create_world_view_operation,
    nh_relate_world_view_operation,
    nh_create_asset_operation,
    nh_delete_asset_operation,
    nh_transfer_asset_operation,
    nh_create_asset_order_operation,
    nh_cancel_asset_order_operation,
    fill_nh_asset_order,
    create_file,
    add_file_relate_account,
    file_signature,
    relate_parent_file,
    contract_revise_operation,
    crontab_create,
    crontab_cancel,
    crontab_recover,
    update_collateral_for_gas,
    account_transfer_operation,
    account_whitelist_operation,
    asset_fund_fee_pool_operation,
    withdraw_permission_create_operation,
    withdraw_permission_update_operation,
    withdraw_permission_claim_operation,
    withdraw_permission_delete_operation,
    custom_operation,
    assert_operation,
    override_transfer_operation,
    transfer_to_blind_operation,
    blind_transfer_operation,
    transfer_from_blind_operation,
    fba_distribute_operation,
    asset_claim_pool_operation,
    asset_update_issuer_operation
    > operation;

} // namespace protocol

} // namespace bcx

FC_REFLECT_TYPENAME( bcx::protocol::operation )

