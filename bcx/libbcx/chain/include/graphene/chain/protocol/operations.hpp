/*
 * Copyright (c) 2015 Cryptonomex, Inc., and contributors.
 *
 * The MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#pragma once
#include <graphene/chain/protocol/base.hpp>
#include <graphene/chain/protocol/account.hpp>
#include <graphene/chain/protocol/assert.hpp>
#include <graphene/chain/protocol/asset_ops.hpp>
#include <graphene/chain/protocol/balance.hpp>
#include <graphene/chain/protocol/custom.hpp>
#include <graphene/chain/protocol/committee_member.hpp>
#include <graphene/chain/protocol/confidential.hpp>
#include <graphene/chain/protocol/fba.hpp>
#include <graphene/chain/protocol/market.hpp>
#include <graphene/chain/protocol/proposal.hpp>
#include <graphene/chain/protocol/transfer.hpp>
#include <graphene/chain/protocol/vesting.hpp>
#include <graphene/chain/protocol/withdraw_permission.hpp>
#include <graphene/chain/protocol/witness.hpp>
#include <graphene/chain/protocol/worker.hpp>

#include "bcx/protocol/contract_create_operation.hpp"
#include "bcx/protocol/contract_call_function_operation.hpp"
#include "bcx/protocol/nh_register_asset_creator_operation.hpp"
#include "bcx/protocol/nh_create_world_view_operation.hpp"
#include "bcx/protocol/nh_relate_world_view_operation.hpp"
#include "bcx/protocol/nh_create_asset_operation.hpp"
#include "bcx/protocol/nh_relate_asset_operation.hpp"
#include "bcx/protocol/nh_delete_asset_operation.hpp"
#include "bcx/protocol/nh_transfer_asset_operation.hpp"
#include "bcx/protocol/nh_create_asset_order_operation.hpp"
#include "bcx/protocol/nh_cancel_asset_order_operation.hpp"
#include "bcx/protocol/contract_revise_operation.hpp"
#include "bcx/protocol/asset_update_restricted.hpp"
#include "bcx/protocol/temporary_authority_chang.hpp"
#include "bcx/protocol/crontab_create.hpp"
#include "bcx/protocol/crontab_cancel.hpp"
#include "bcx/protocol/crontab_recover.hpp"
#include "bcx/protocol/update_collateral_for_gas.hpp"


#include "bcx/protocol/fill_nh_asset_order.hpp"
#include "bcx/protocol/create_file.hpp"
#include "bcx/protocol/add_file_relate_account.hpp"
#include "bcx/protocol/file_signature.hpp"
#include "bcx/protocol/relate_parent_file.hpp"


namespace graphene { namespace chain {
   /**
    * @ingroup operations
    *
    * Defines the set of valid operations as a discriminated union type.
    */
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
            bcx::protocol::asset_update_restricted,
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
            bcx::protocol::contract_create_operation,
            bcx::protocol::contract_call_function_operation,
            bcx::protocol::temporary_authority_chang,
            bcx::protocol::nh_register_asset_creator_operation,
            bcx::protocol::nh_create_world_view_operation,
            bcx::protocol::nh_relate_world_view_operation,
            bcx::protocol::nh_create_asset_operation,
            bcx::protocol::nh_delete_asset_operation,
            bcx::protocol::nh_transfer_asset_operation,
            bcx::protocol::nh_create_asset_order_operation,
            bcx::protocol::nh_cancel_asset_order_operation,
            bcx::protocol::fill_nh_asset_order,
            bcx::protocol::create_file,
            bcx::protocol::add_file_relate_account,
            bcx::protocol::file_signature,
            bcx::protocol::relate_parent_file,
            bcx::protocol::contract_revise_operation,
            bcx::protocol::crontab_create,
            bcx::protocol::crontab_cancel,
            bcx::protocol::crontab_recover,
            bcx::protocol::update_collateral_for_gas,
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

   /// @} // operations group

   /**
    *  Appends required authorites to the result vector.  The authorities appended are not the
    *  same as those returned by get_required_auth 
    *
    *  @return a set of required authorities for @ref op
    */
   void operation_get_required_authorities( const operation& op, 
                                            flat_set<account_id_type>& active,
                                            flat_set<account_id_type>& owner,
                                            vector<authority>&  other );

   void operation_validate( const operation& op );

   /**
    *  @brief necessary to support nested operations inside the proposal_create_operation
    */
   struct op_wrapper
   {
      public:
         op_wrapper(const operation& op = operation()):op(op){}
         operation op;
   };

} } // graphene::chain

FC_REFLECT_TYPENAME( graphene::chain::operation )
FC_REFLECT( graphene::chain::op_wrapper, (op) )
