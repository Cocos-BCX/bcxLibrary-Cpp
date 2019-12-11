#pragma once

#include "../fc/include/fc/crypto/elliptic.hpp"
#include "../fc/include/fc/crypto/base58.hpp"
#include "../fc/include/fc/crypto/ripemd160.hpp"
#include "./object.hpp"

/*
#pragma once

namespace bcx {

namespace protocol {

} // namespace protocol

} // namespace bcx
*/

#pragma once

//namespace fc { namespace ecc {
//    class public_key;
//    typedef fc::array<char,33>  public_key_data;
//} } // fc::ecc

namespace bcx {

namespace protocol {

struct void_t{};

typedef fc::ecc::private_key        private_key_type;
typedef fc::sha256 chain_id_type;

enum asset_issuer_permission_flags {
   charge_market_fee    = 0x01, /**< an issuer-specified percentage of all market trades in this asset is paid to the issuer */
   white_list           = 0x02, /**< accounts must be whitelisted in order to hold this asset */
   override_authority   = 0x04, /**< issuer may transfer asset back to himself */
   transfer_restricted  = 0x08, /**< require the issuer to be one party to every transfer */
   disable_force_settle = 0x10, /**< disable force settling */
   global_settle        = 0x20, /**< allow the bitasset issuer to force a global settling -- this may be set in permissions, but not flags */
   disable_confidential = 0x40, /**< allow the asset to be used with confidential transactions */
   witness_fed_asset    = 0x80, /**< allow the asset to be fed by witnesses */
   committee_fed_asset  = 0x100 /**< allow the asset to be fed by the committee */
};

const static uint32_t ASSET_ISSUER_PERMISSION_MASK = charge_market_fee|white_list|override_authority|transfer_restricted|disable_force_settle|global_settle|disable_confidential
   |witness_fed_asset|committee_fed_asset;
const static uint32_t UIA_ASSET_ISSUER_PERMISSION_MASK = charge_market_fee|white_list|override_authority|transfer_restricted|disable_confidential;

enum reserved_spaces {
   relative_protocol_ids = 0,
   protocol_ids          = 1,
   implementation_ids    = 2,
   game_protocol_ids     = 4
};

/**
 *  List all object types from all namespaces here so they can
 *  be easily reflected and displayed in debug output.  If a 3rd party
 *  wants to extend the core code then they will have to change the
 *  packed_object::type field from enum_type to uint16 to avoid
 *  warnings when converting packed_objects to/from json.
 */
enum object_type {
   null_object_type,
   base_object_type,
   account_object_type,
   asset_object_type,
   force_settlement_object_type,
   committee_member_object_type,
   witness_object_type,
   limit_order_object_type,
   call_order_object_type,
   custom_object_type,
   proposal_object_type,
   operation_history_object_type,
   withdraw_permission_object_type,
   vesting_balance_object_type,
   worker_object_type,
   balance_object_type,
    contract_object_type,
   OBJECT_TYPE_COUNT ///< Sentry value which contains the number of different object types
};

enum impl_object_type {
   impl_global_property_object_type,
   impl_dynamic_global_property_object_type,
   impl_reserved0_object_type,      // formerly index_meta_object_type, TODO: delete me
   impl_asset_dynamic_data_type,
   impl_asset_bitasset_data_type,
   impl_account_balance_object_type,
   impl_account_statistics_object_type,
   impl_transaction_object_type,
   impl_block_summary_object_type,
   impl_account_transaction_history_object_type,
   impl_blinded_balance_object_type,
   impl_chain_property_object_type,
   impl_witness_schedule_object_type,
   impl_budget_record_object_type,
   impl_special_authority_object_type,
   impl_buyback_object_type,
   impl_fba_accumulator_object_type,
   impl_collateral_bid_object_type
};
 
 enum game_object_type {
     game_developer_object_type,
     game_version_object_type,
     game_item_object_type,
     game_item_order_object_type
 };


class account_object;
class asset_object;

typedef object_id< protocol_ids, account_object_type,            account_object>               account_id_type;
typedef object_id< protocol_ids, asset_object_type,              asset_object>                 asset_id_type;
//typedef object_id< protocol_ids, force_settlement_object_type,   force_settlement_object>      force_settlement_id_type;
//typedef object_id< protocol_ids, committee_member_object_type,   committee_member_object>      committee_member_id_type;
//typedef object_id< protocol_ids, witness_object_type,            witness_object>               witness_id_type;
//typedef object_id< protocol_ids, limit_order_object_type,        limit_order_object>           limit_order_id_type;
//typedef object_id< protocol_ids, call_order_object_type,         call_order_object>            call_order_id_type;
//typedef object_id< protocol_ids, custom_object_type,             custom_object>                custom_id_type;
//typedef object_id< protocol_ids, proposal_object_type,           proposal_object>              proposal_id_type;
//typedef object_id< protocol_ids, operation_history_object_type,  operation_history_object>     operation_history_id_type;
//typedef object_id< protocol_ids, withdraw_permission_object_type,withdraw_permission_object>   withdraw_permission_id_type;
//typedef object_id< protocol_ids, vesting_balance_object_type,    vesting_balance_object>       vesting_balance_id_type;
//typedef object_id< protocol_ids, worker_object_type,             worker_object>                worker_id_type;
//typedef object_id< protocol_ids, balance_object_type,            balance_object>               balance_id_type;
//typedef object_id< protocol_ids, contract_object_type, bcx::protocol::contract_object>               contract_id_type;
//typedef object_id< game_protocol_ids, game_item_object_type, bcx::protocol::game_protocol_object> game_protocol_id_type;

class asset_dynamic_data_object;
class asset_bitasset_data_object;

typedef object_id< implementation_ids, impl_asset_dynamic_data_type,      asset_dynamic_data_object>                 asset_dynamic_data_id_type;
typedef object_id< implementation_ids, impl_asset_bitasset_data_type,     asset_bitasset_data_object>                asset_bitasset_data_id_type;

typedef fc::ripemd160                                        block_id_type;
typedef fc::sha256                                           digest_type;
typedef fc::ecc::compact_signature                           signature_type;
typedef fc::safe<int64_t>                                    share_type;
typedef uint16_t                                             weight_type;

struct public_key_type {
    struct binary_key {
       binary_key() {}
       uint32_t                 check = 0;
       fc::ecc::public_key_data data;
    };

    fc::ecc::public_key_data key_data;
    public_key_type();
    public_key_type( const fc::ecc::public_key_data& data );
    public_key_type( const fc::ecc::public_key& pubkey );
    explicit public_key_type( const std::string& base58str );
    operator fc::ecc::public_key_data() const;
    operator fc::ecc::public_key() const;
    explicit operator std::string() const;
    bool operator < (const public_key_type & cmp) const;
    friend bool operator == ( const public_key_type& p1, const fc::ecc::public_key& p2);
    friend bool operator == ( const public_key_type& p1, const public_key_type& p2);
    friend bool operator != ( const public_key_type& p1, const public_key_type& p2);
};

} // namespace bcx

} // namespace protocol

namespace fc {
    void to_variant( const bcx::protocol::public_key_type& var,  fc::variant& vo, uint32_t max_depth = 2 );
    void from_variant( const fc::variant& var,  bcx::protocol::public_key_type& vo, uint32_t max_depth = 2 );
}

FC_REFLECT( bcx::protocol::public_key_type, (key_data) )
FC_REFLECT( bcx::protocol::public_key_type::binary_key, (data)(check) )

FC_REFLECT( bcx::protocol::void_t, )

