#pragma once

#include "../types.hpp"
#include "../memo.hpp"
#include "../asset.hpp"

namespace bcx {

namespace protocol {

struct transfer_operation {
    /// Account to transfer asset from
    account_id_type  from;
    /// Account to transfer asset to
    account_id_type  to;
    /// The amount of asset to transfer from @ref from to @ref to
    asset            amount;
    /// User provided data encrypted to the memo key of the "to" account
    fc::optional<memo_data> memo;
    extensions_type   extensions;
};

} // namespace protocol

} // namespace bcx

FC_REFLECT( bcx::protocol::transfer_operation,
           (from)(to)(amount)(memo)(extensions)
           )

