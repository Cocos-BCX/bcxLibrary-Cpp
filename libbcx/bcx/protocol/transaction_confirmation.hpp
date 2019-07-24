//
//  transaction_confirmation.hpp
//  bcx
//
//  Created by hugo on 2019/4/17.
//  Copyright © 2019 SDKBox. All rights reserved.
//

#ifndef transaction_confirmation_hpp
#define transaction_confirmation_hpp

#include "graphene/chain/protocol/base.hpp"
#include "graphene/chain/protocol/transaction.hpp"

namespace bcx { namespace protocol {

struct operation_result_running_time {
    uint32_t real_running_time;
};

struct operation_result_game_id_type {
    graphene::chain::game_protocol_id_type result;
    uint32_t real_running_time;
};

typedef fc::static_variant<
    graphene::chain::void_result,
    operation_result_running_time,
    operation_result_game_id_type
> operation_result;

struct processed_transaction : public graphene::chain::signed_transaction {
    processed_transaction( const graphene::chain::signed_transaction& trx = graphene::chain::signed_transaction() )
    : graphene::chain::signed_transaction(trx){}

    std::vector<bcx::protocol::operation_result> operation_results;
};

struct transaction_confirmation {
    graphene::chain::transaction_id_type   id;
    uint32_t                               block_num;
    uint32_t                               trx_num;
    processed_transaction                  trx;
};

} }

FC_REFLECT( bcx::protocol::operation_result_running_time,
           (real_running_time)
           )

FC_REFLECT( bcx::protocol::operation_result_game_id_type,
           (result)
           (real_running_time)
           )

FC_REFLECT_DERIVED( bcx::protocol::processed_transaction,
                   (graphene::chain::signed_transaction),
                   (operation_results)
                   )

FC_REFLECT( bcx::protocol::transaction_confirmation,
           (id)
           (block_num)
           (trx_num)
           (trx)
           )

#endif /* transaction_confirmation_hpp */
