
#pragma once

#include "fc/crypto/elliptic.hpp"
#include "fc/rpc/state.hpp"
#include "fc/io/json.hpp"
#include "fc/rpc/state.hpp"
#include "fc/reflect/variant.hpp"
#include <fc/smart_ref_fwd.hpp>
#include <fc/smart_ref_impl.hpp>
#include <fc/crypto/base58.hpp>
#include <fc/time.hpp>
#include <fc/reflect/reflect.hpp>

//namespace bcx {

//    struct login
//    {
//        std::string name = "";
//        std::string pwd = "";
//    };
////    FC_REFLECT(login, (name)(pwd))
//
//    struct database {
//    };
//    struct network_broadcast {
//    };
//    struct history {
//    };
//    struct get_chain_id {
//    };
//    struct get_objects {
//    };
//    struct set_subscribe_callback {
//    };
//    struct get_key_references {
//    };
//    struct get_full_accounts {
//    };
//    struct lookup_asset_symbols {
//    };
//    struct get_potential_signatures {
//    };
//    struct get_potential_address_signatures {
//    };
//    struct get_required_signatures {
//    };
//    struct get_required_fees {
//    };
//    struct broadcast_transaction_with_callback {
//    };
//    
//    
//    
//    // receive data
//    struct object_type
//    {
//        std::string id ; //"2.1.0",
//        int64_t head_block_number; //1617308,
//        std::string head_block_id; //":"0018ad9ca1d553192ef3fb2a119086d10aab8bf2",
//        fc::time_point_sec time;// ":"2019-03-12T03:15:50",
//        std::string current_witness;//":"1.6.3",
//        int current_transaction_count;//":0,
//        fc::time_point_sec next_maintenance_time;//":"2019-03-12T03:20:00",
//        fc::time_point_sec last_budget_time;//":"2019-03-12T03:10:00",
//        int64_t witness_budget;//":25300000,
//        int accounts_registered_this_interval;//":0,
//        int recently_missed_count;//":0,
//        int current_aslot;//":7309584,
//        std::string recent_slots_filled;//":"340282366920938463463374607431768211455",
//        int dynamic_flags;//":0,
//        int64_t last_irreversible_block_num;//":1617301
//    };
//    FC_REFLECT(object_type,
//               (id)
//               (head_block_number)
//               (head_block_id)
//               (time)
//               (current_witness)
//               (current_transaction_count)
//               (next_maintenance_time)
//               (last_budget_time)
//               (witness_budget)
//               (accounts_registered_this_interval)
//               (recently_missed_count)
//               (current_aslot)
//               (recent_slots_filled)
//               (dynamic_flags)
//               (last_irreversible_block_num))
//} // bcx namespace
