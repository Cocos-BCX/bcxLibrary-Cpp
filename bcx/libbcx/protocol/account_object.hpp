#pragma once

#include "./types.hpp"
#include "./authority.hpp"
#include "../fc/include/fc/time.hpp"
#include "./asset_ops.hpp"

namespace bcx {

namespace protocol {

class account_object : public bcx::protocol::abstract_object<account_object> {
public:
    static const uint8_t space_id = protocol_ids;
    static const uint8_t type_id  = account_object_type;

    //      time_point_sec membership_expiration_date;

    ///The account that paid the fee to register this account. Receives a percentage of referral rewards.
    account_id_type registrar;
    /// The account credited as referring this account. Receives a percentage of referral rewards.
    account_id_type referrer;
    /// The lifetime member at the top of the referral tree. Receives a percentage of referral rewards.
    account_id_type lifetime_referrer;

    /// Percentage of fee which should go to network.
    uint16_t network_fee_percentage = 0;
    /// Percentage of fee which should go to lifetime referrer.
    uint16_t lifetime_referrer_fee_percentage = 0;
    /// Percentage of referral rewards (leftover fee after paying network and lifetime referrer) which should go
    /// to referrer. The remainder of referral rewards goes to the registrar.
    uint16_t referrer_rewards_percentage = 0;

    /// The account's name. This name must be unique among all account names on the graph. May not be empty.
    std::string name;

    /**
    * The owner authority represents absolute control over the account. Usually the keys in this authority will
    * be kept in cold storage, as they should not be needed very often and compromise of these keys constitutes
    * complete and irrevocable loss of the account. Generally the only time the owner authority is required is to
    * update the active authority.
    */
    authority owner;
    /// The owner authority contains the hot keys of the account. This authority has control over nearly all
    /// operations the account may perform.
    authority active;

    account_id_type get_id()const { return id; }
};

class asset_bitasset_data_object : public abstract_object<asset_bitasset_data_object> {
public:
    static const uint8_t space_id = implementation_ids;
    static const uint8_t type_id  = impl_asset_bitasset_data_type;

    /// The asset this object belong to
    asset_id_type asset_id;

    /// The tunable options for BitAssets are stored in this field.
    bitasset_options options;

    /// Feeds published for this asset. If issuer is not committee, the keys in this map are the feed publishing
    /// accounts; otherwise, the feed publishers are the currently active committee_members and witnesses and this map
    /// should be treated as an implementation detail. The timestamp on each feed is the time it was published.
    std::map<account_id_type, std::pair<fc::time_point_sec,price_feed>> feeds;
    /// This is the currently active price feed, calculated as the median of values from the currently active
    /// feeds.
    price_feed current_feed;
    /// This is the publication time of the oldest feed which was factored into current_feed.
    fc::time_point_sec current_feed_publication_time;

    /// True if this asset implements a @ref prediction_market
    bool is_prediction_market = false;

    /// This is the volume of this asset which has been force-settled this maintanence interval
    share_type force_settled_volume;

    /**
    *  In the event of a black swan, the swan price is saved in the settlement price, and all margin positions
    *  are settled at the same price with the siezed collateral being moved into the settlement fund. From this
    *  point on no further updates to the asset are permitted (no feeds, etc) and forced settlement occurs
    *  immediately when requested, using the settlement price and fund.
    */
    ///@{
    /// Price at which force settlements of a black swanned asset will occur
    price settlement_price;
    /// Amount of collateral which is available for force settlement
    share_type settlement_fund;
    ///@}

    /// Track whether core_exchange_rate in corresponding asset_object has updated
    bool asset_cer_updated = false;

    /// Track whether core exchange rate in current feed has updated
    bool feed_cer_updated = false;
};

} // namespace protocol

} // namespace bcx


FC_REFLECT_DERIVED( bcx::protocol::asset_bitasset_data_object, (bcx::protocol::object),
    (asset_id)
    (feeds)
    (current_feed)
    (current_feed_publication_time)
    (options)
    (force_settled_volume)
    (is_prediction_market)
    (settlement_price)
    (settlement_fund)
    (asset_cer_updated)
    (feed_cer_updated)
)

FC_REFLECT_DERIVED( bcx::protocol::account_object, (bcx::protocol::object),
    (registrar)(referrer)(lifetime_referrer)
    (network_fee_percentage)(lifetime_referrer_fee_percentage)(referrer_rewards_percentage)
    (name)(owner)(active)
)

