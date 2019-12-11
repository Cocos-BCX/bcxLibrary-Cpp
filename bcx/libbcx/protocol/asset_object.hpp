#pragma once

#include "./types.hpp"

#pragma once

namespace bcx {

namespace protocol {

class asset_dynamic_data_object : public abstract_object<asset_dynamic_data_object> {
public:
    static const uint8_t space_id = implementation_ids;
    static const uint8_t type_id  = impl_asset_dynamic_data_type;

    /// The number of shares currently in existence
    share_type current_supply;
    share_type confidential_supply; ///< total asset held in confidential balances
    share_type accumulated_fees; ///< fees accumulate to be paid out over time
    share_type fee_pool;         ///< in core asset
};

class asset_object : public bcx::protocol::abstract_object<asset_object> {
public:
    /// Ticker symbol for this asset, i.e. "USD"
    std::string symbol;
    /// Maximum number of digits after the decimal point (must be <= 12)
    uint8_t precision = 0;
    /// ID of the account which issued this asset.
    account_id_type issuer;

    asset_options options;

    /// Current supply, fee pool, and collected fees are stored in a separate object as they change frequently.
    asset_dynamic_data_id_type  dynamic_asset_data_id;
    /// Extra data associated with BitAssets. This field is non-null if and only if is_market_issued() returns true
    fc::optional<asset_bitasset_data_id_type> bitasset_data_id;

    fc::optional<account_id_type> buyback_account;
    
    asset_id_type get_id()const { return id; }
};

} // namespace protocol

} // namespace bcx


FC_REFLECT_DERIVED( bcx::protocol::asset_dynamic_data_object, (bcx::protocol::object),
                   (current_supply)(confidential_supply)(accumulated_fees)(fee_pool)
                   )

FC_REFLECT_DERIVED( bcx::protocol::asset_object, (bcx::protocol::object),
                (symbol)
                (precision)
                (issuer)
                (options)
                (dynamic_asset_data_id)
                (bitasset_data_id)
                (buyback_account)
)

