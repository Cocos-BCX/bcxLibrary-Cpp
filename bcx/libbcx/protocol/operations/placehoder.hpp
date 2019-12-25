#pragma once

#include "../types.hpp"
#include "../memo.hpp"
#include "../asset.hpp"

namespace bcx {

namespace protocol {

//empty, just take place hodler

struct asset_update_restricted {};
struct asset_update_bitasset_operation {};
struct asset_update_feed_producers_operation {};
struct asset_settle_operation {};
struct asset_global_settle_operation {};
struct asset_publish_feed_operation {};
struct witness_create_operation {};
struct witness_update_operation {};
struct proposal_create_operation {};
struct proposal_update_operation {};
struct proposal_delete_operation {};
struct committee_member_create_operation {};
struct committee_member_update_operation {};
struct committee_member_update_global_parameters_operation {};
struct vesting_balance_create_operation {};
struct vesting_balance_withdraw_operation {};
struct worker_create_operation {};
struct balance_claim_operation {};
struct asset_settle_cancel_operation {};
struct asset_claim_fees_operation {};
struct bid_collateral_operation {};
struct execute_bid_operation {};
struct temporary_authority_chang {};
struct nh_register_asset_creator_operation {};
struct nh_create_world_view_operation {};
struct nh_relate_world_view_operation {};
struct nh_create_asset_operation {};
struct nh_delete_asset_operation {};
struct nh_transfer_asset_operation {};
struct nh_create_asset_order_operation {};
struct nh_cancel_asset_order_operation {};
struct fill_nh_asset_order {};
struct create_file {};
struct add_file_relate_account {};
struct file_signature {};
struct relate_parent_file {};
struct crontab_create {};
struct crontab_cancel {};
struct crontab_recover {};
struct update_collateral_for_gas {};
struct account_transfer_operation {};
struct account_whitelist_operation {};
struct asset_fund_fee_pool_operation {};
struct withdraw_permission_create_operation {};
struct withdraw_permission_update_operation {};
struct withdraw_permission_claim_operation {};
struct withdraw_permission_delete_operation {};
struct custom_operation {};
struct assert_operation {};
struct override_transfer_operation {};
struct transfer_to_blind_operation {};
struct blind_transfer_operation {};
struct transfer_from_blind_operation {};
struct fba_distribute_operation {};
struct asset_claim_pool_operation {};
struct asset_update_issuer_operation {};



} // namespace protocol

} // namespace bcx


FC_REFLECT( bcx::protocol::asset_update_restricted, )
FC_REFLECT( bcx::protocol::asset_update_bitasset_operation, )
FC_REFLECT( bcx::protocol::asset_update_feed_producers_operation, )
FC_REFLECT( bcx::protocol::asset_settle_operation, )
FC_REFLECT( bcx::protocol::asset_global_settle_operation, )
FC_REFLECT( bcx::protocol::asset_publish_feed_operation, )
FC_REFLECT( bcx::protocol::witness_create_operation, )
FC_REFLECT( bcx::protocol::witness_update_operation, )
FC_REFLECT( bcx::protocol::proposal_create_operation, )
FC_REFLECT( bcx::protocol::proposal_update_operation, )
FC_REFLECT( bcx::protocol::proposal_delete_operation, )
FC_REFLECT( bcx::protocol::committee_member_create_operation, )
FC_REFLECT( bcx::protocol::committee_member_update_operation, )
FC_REFLECT( bcx::protocol::committee_member_update_global_parameters_operation, )
FC_REFLECT( bcx::protocol::vesting_balance_create_operation, )
FC_REFLECT( bcx::protocol::vesting_balance_withdraw_operation, )
FC_REFLECT( bcx::protocol::worker_create_operation, )
FC_REFLECT( bcx::protocol::balance_claim_operation, )
FC_REFLECT( bcx::protocol::asset_settle_cancel_operation, )
FC_REFLECT( bcx::protocol::asset_claim_fees_operation, )
FC_REFLECT( bcx::protocol::bid_collateral_operation, )
FC_REFLECT( bcx::protocol::execute_bid_operation, )
FC_REFLECT( bcx::protocol::temporary_authority_chang, )
FC_REFLECT( bcx::protocol::nh_register_asset_creator_operation, )
FC_REFLECT( bcx::protocol::nh_create_world_view_operation, )
FC_REFLECT( bcx::protocol::nh_relate_world_view_operation, )
FC_REFLECT( bcx::protocol::nh_create_asset_operation, )
FC_REFLECT( bcx::protocol::nh_delete_asset_operation, )
FC_REFLECT( bcx::protocol::nh_transfer_asset_operation, )
FC_REFLECT( bcx::protocol::nh_create_asset_order_operation, )
FC_REFLECT( bcx::protocol::nh_cancel_asset_order_operation, )
FC_REFLECT( bcx::protocol::fill_nh_asset_order, )
FC_REFLECT( bcx::protocol::create_file, )
FC_REFLECT( bcx::protocol::add_file_relate_account, )
FC_REFLECT( bcx::protocol::file_signature, )
FC_REFLECT( bcx::protocol::relate_parent_file, )
FC_REFLECT( bcx::protocol::crontab_create, )
FC_REFLECT( bcx::protocol::crontab_cancel, )
FC_REFLECT( bcx::protocol::crontab_recover, )
FC_REFLECT( bcx::protocol::update_collateral_for_gas, )
FC_REFLECT( bcx::protocol::account_transfer_operation, )
FC_REFLECT( bcx::protocol::account_whitelist_operation, )
FC_REFLECT( bcx::protocol::asset_fund_fee_pool_operation, )
FC_REFLECT( bcx::protocol::withdraw_permission_create_operation, )
FC_REFLECT( bcx::protocol::withdraw_permission_update_operation, )
FC_REFLECT( bcx::protocol::withdraw_permission_claim_operation, )
FC_REFLECT( bcx::protocol::withdraw_permission_delete_operation, )
FC_REFLECT( bcx::protocol::custom_operation, )
FC_REFLECT( bcx::protocol::assert_operation, )
FC_REFLECT( bcx::protocol::override_transfer_operation, )
FC_REFLECT( bcx::protocol::transfer_to_blind_operation, )
FC_REFLECT( bcx::protocol::blind_transfer_operation, )
FC_REFLECT( bcx::protocol::transfer_from_blind_operation, )
FC_REFLECT( bcx::protocol::fba_distribute_operation, )
FC_REFLECT( bcx::protocol::asset_claim_pool_operation, )
FC_REFLECT( bcx::protocol::asset_update_issuer_operation, )

