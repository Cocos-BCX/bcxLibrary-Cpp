#pragma once

#include "./types.hpp"
#include "./config.hpp"

namespace bcx {

namespace protocol {

struct asset {
    asset( share_type a = 0, asset_id_type id = asset_id_type() ):amount(a),asset_id(id){}

    share_type    amount;
    asset_id_type asset_id;
};

struct price {
   explicit price(const asset& _base = asset(), const asset& _quote = asset())
      : base(_base),quote(_quote){}

   asset base;
   asset quote;
};

struct price_feed {
   /**
    *  Required maintenance collateral is defined
    *  as a fixed point number with a maximum value of 10.000
    *  and a minimum value of 1.000.  (denominated in GRAPHENE_COLLATERAL_RATIO_DENOM)
    *
    *  A black swan event occurs when value_of_collateral equals
    *  value_of_debt, to avoid a black swan a margin call is
    *  executed when value_of_debt * required_maintenance_collateral
    *  equals value_of_collateral using rate.
    *
    *  Default requirement is $1.75 of collateral per $1 of debt
    *
    *  BlackSwan ---> SQR ---> MCR ----> SP
    */
   ///@{
   /**
    * Forced settlements will evaluate using this price, defined as BITASSET / COLLATERAL
    */
   price settlement_price;

   /// Price at which automatically exchanging this asset for CORE from fee pool occurs (used for paying fees)
   price core_exchange_rate;

   /** Fixed point between 1.000 and 10.000, implied fixed point denominator is GRAPHENE_COLLATERAL_RATIO_DENOM */
   uint16_t maintenance_collateral_ratio = BCX_DEFAULT_MAINTENANCE_COLLATERAL_RATIO;

   /** Fixed point between 1.000 and 10.000, implied fixed point denominator is GRAPHENE_COLLATERAL_RATIO_DENOM */
   uint16_t maximum_short_squeeze_ratio = BCX_DEFAULT_MAX_SHORT_SQUEEZE_RATIO;

   /**
    *  When updating a call order the following condition must be maintained:
    *
    *  debt * maintenance_price() < collateral
    *  debt * settlement_price    < debt * maintenance
    *  debt * maintenance_price() < debt * max_short_squeeze_price()
   price maintenance_price()const;
    */

};

} // namespace protocol

} // namespace bcx


FC_REFLECT( bcx::protocol::asset, (amount)(asset_id) )
FC_REFLECT( bcx::protocol::price, (base)(quote) )

#define BCX_PRICE_FEED_FIELDS (settlement_price)(maintenance_collateral_ratio)(maximum_short_squeeze_ratio) \
   (core_exchange_rate)

FC_REFLECT( bcx::protocol::price_feed, BCX_PRICE_FEED_FIELDS )

