#pragma once

#include "../../fc/include/fc/optional.hpp"
#include "../types.hpp"
#include "../memo.hpp"
#include "../asset.hpp"
#include "../authority.hpp"
#include "../buyback.hpp"
#include "../ext.hpp"
#include "../special_authority.hpp"
#include "../vote.hpp"

namespace bcx {

namespace protocol {

struct account_options {
    public_key_type  memo_key;

    std::set<vote_id_type> votes;
    extensions_type        extensions;
};

struct account_create_operation {
    struct ext {
        fc::optional< void_t >            null_ext;
        fc::optional< special_authority > owner_special_authority;
        fc::optional< special_authority > active_special_authority;
        fc::optional< buyback_account_options > buyback_options;
    };

    /// This account pays the fee. Must be a lifetime member.
    account_id_type registrar;

    std::string          name;
    authority       owner;
    authority       active;

    account_options options;
    extension< ext > extensions;
};

struct account_update_operation {
    struct ext {
        fc::optional< void_t >            null_ext;
        fc::optional< special_authority > owner_special_authority;
        fc::optional< special_authority > active_special_authority;
    };

    fc::optional<std::pair<uint32_t, asset>>  lock_with_vote;
    /// The account to update
    account_id_type account;

    /// New owner authority. If set, this operation requires owner authority to execute.
    fc::optional<authority> owner;
    /// New active authority. This can be updated by the current active authority.
    fc::optional<authority> active;

    /// New account options
    fc::optional<account_options> new_options;
    extension< ext > extensions;
};

struct account_upgrade_operation {
    /// The account to upgrade; must not already be a lifetime member
    account_id_type   account_to_upgrade;
    /// If true, the account will be upgraded to a lifetime member; otherwise, it will add a year to the subscription
    bool              upgrade_to_lifetime_member = false;
    extensions_type   extensions;
};

} // namespace protocol

} // namespace bcx

FC_REFLECT(bcx::protocol::account_options,
           (memo_key)(votes)(extensions)
)

FC_REFLECT( bcx::protocol::account_create_operation::ext,
            (null_ext)(owner_special_authority)(active_special_authority)(buyback_options)
)

FC_REFLECT( bcx::protocol::account_create_operation,
            (registrar)(name)(owner)(active)(options)(extensions)
)

FC_REFLECT( bcx::protocol::account_update_operation::ext,
            (null_ext)(owner_special_authority)(active_special_authority)
)

FC_REFLECT( bcx::protocol::account_update_operation,
            (lock_with_vote)(account)(owner)(active)(new_options)(extensions)
)

FC_REFLECT( bcx::protocol::account_upgrade_operation,
            (account_to_upgrade)(upgrade_to_lifetime_member)(extensions)
)

