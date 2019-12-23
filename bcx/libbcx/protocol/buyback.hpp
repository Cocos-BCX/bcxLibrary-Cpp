#pragma once

#include "./types.hpp"

namespace bcx { namespace protocol {

struct buyback_account_options {
    /**
    * The asset to buy.
    */
    asset_id_type       asset_to_buy;

    /**
    * Issuer of the asset.  Must sign the transaction, must match issuer
    * of specified asset.
    */
    account_id_type     asset_to_buy_issuer;

    /**
    * What assets the account is willing to buy with.
    * Other assets will just sit there since the account has null authority.
    */
    std::set< asset_id_type > markets;
};

} }

FC_REFLECT( bcx::protocol::buyback_account_options,
           (asset_to_buy)(asset_to_buy_issuer)(markets)
)

