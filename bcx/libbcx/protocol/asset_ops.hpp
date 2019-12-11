#pragma once

#include "./base.hpp"
#include "./config.hpp"
#include "./types.hpp"
#include "./asset.hpp"

#pragma once

namespace bcx {

namespace protocol {

struct asset_options {
   /// The maximum supply of this asset which may exist at any given time. This can be as large as
   /// GRAPHENE_MAX_SHARE_SUPPLY
   share_type max_supply = BCX_MAX_SHARE_SUPPLY;
   /// When this asset is traded on the markets, this percentage of the total traded will be exacted and paid
   /// to the issuer. This is a fixed point value, representing hundredths of a percent, i.e. a value of 100
   /// in this field means a 1% fee is charged on market trades of this asset.
   uint16_t market_fee_percent = 0;
   /// Market fees calculated as @ref market_fee_percent of the traded volume are capped to this value
   share_type max_market_fee = BCX_MAX_SHARE_SUPPLY;

   /// The flags which the issuer has permission to update. See @ref asset_issuer_permission_flags
   uint16_t issuer_permissions = UIA_ASSET_ISSUER_PERMISSION_MASK;
   /// The currently active flags on this permission. See @ref asset_issuer_permission_flags
   uint16_t flags = 0;

   /// When a non-core asset is used to pay a fee, the blockchain must convert that asset to core asset in
   /// order to accept the fee. If this asset's fee pool is funded, the chain will automatically deposite fees
   /// in this asset to its accumulated fees, and withdraw from the fee pool the same amount as converted at
   /// the core exchange rate.
   fc::optional<price> core_exchange_rate = price(asset(), asset(0, asset_id_type(1)));

   /// A set of accounts which maintain whitelists to consult for this asset. If whitelist_authorities
   /// is non-empty, then only accounts in whitelist_authorities are allowed to hold, use, or transfer the asset.
   // flat_set<account_id_type> whitelist_authorities;
   /// A set of accounts which maintain blacklists to consult for this asset. If flags & white_list is set,
   /// an account may only send, receive, trade, etc. in this asset if none of these accounts appears in
   /// its account_object::blacklisting_accounts field. If the account is blacklisted, it may not transact in
   /// this asset even if it is also whitelisted.
   // flat_set<account_id_type> blacklist_authorities;

   /** defines the assets that this asset may be traded against in the market */
   // flat_set<asset_id_type>   whitelist_markets;
   /** defines the assets that this asset may not be traded against in the market, must not overlap whitelist */
   // flat_set<asset_id_type>   blacklist_markets;

   /**
    * data that describes the meaning/purpose of this asset, fee will be charged proportional to
    * size of description.
    */
   std::string description;
   extensions_type extensions;
};

struct bitasset_options {
   /// Time before a price feed expires
   uint32_t feed_lifetime_sec = BCX_DEFAULT_PRICE_FEED_LIFETIME;
   /// Minimum number of unexpired feeds required to extract a median feed from
   uint8_t minimum_feeds = 1;
   /// This is the delay between the time a long requests settlement and the chain evaluates the settlement
   uint32_t force_settlement_delay_sec = BCX_DEFAULT_FORCE_SETTLEMENT_DELAY;
   /// This is the percent to adjust the feed price in the short's favor in the event of a forced settlement
   uint16_t force_settlement_offset_percent = BCX_DEFAULT_FORCE_SETTLEMENT_OFFSET;
   /// Force settlement volume can be limited such that only a certain percentage of the total existing supply
   /// of the asset may be force-settled within any given chain maintenance interval. This field stores the
   /// percentage of the current supply which may be force settled within the current maintenance interval. If
   /// force settlements come due in an interval in which the maximum volume has already been settled, the new
   /// settlements will be enqueued and processed at the beginning of the next maintenance interval.
   uint16_t maximum_force_settlement_volume = BCX_DEFAULT_FORCE_SETTLEMENT_MAX_VOLUME;
   /// This speicifies which asset type is used to collateralize short sales
   /// This field may only be updated if the current supply of the asset is zero.
   asset_id_type short_backing_asset;
   extensions_type extensions;

};


} // namespace protocol

} // namespace bcx


FC_REFLECT( bcx::protocol::asset_options,
            (max_supply)
            (market_fee_percent)
            (max_market_fee)
            (issuer_permissions)
            (flags)
            (core_exchange_rate)
            (description)
            (extensions)
)

FC_REFLECT( bcx::protocol::bitasset_options,
            (feed_lifetime_sec)
            (minimum_feeds)
            (force_settlement_delay_sec)
            (force_settlement_offset_percent)
            (maximum_force_settlement_volume)
            (short_backing_asset)
            (extensions)
)

