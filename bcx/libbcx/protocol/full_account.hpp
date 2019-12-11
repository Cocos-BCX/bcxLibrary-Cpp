#pragma once

#include "./account_object.hpp"

namespace bcx { namespace protocol {

    struct full_account {
        account_object account;
    };

} }

FC_REFLECT( bcx::protocol::full_account,
            (account)
           )

