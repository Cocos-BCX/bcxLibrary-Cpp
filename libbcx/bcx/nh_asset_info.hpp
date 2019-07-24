//
//  nh_asset_info.h
//  bcx
//
//  Created by hugo on 2019/4/1.
//  Copyright © 2019 SDKBox. All rights reserved.
//

#pragma once

#include "platform/PlatformMacros.h"
#include "fc/reflect/variant.hpp"
#include <graphene/chain/protocol/types.hpp>

NS_BCX_BEGIN

struct nh_asset_create_info {
    std::string asset_name;
    std::string world_view;
    std::string base_describe;
    std::string owner;
};

struct nh_asset_info {
    graphene::chain::game_protocol_id_type id;
    std::string nh_hash;
    std::string base_describe;
    std::string nh_asset_creator;
    std::string nh_asset_owner;
    std::string nh_asset_active;
    std::string authority_account;
    std::string asset_qualifier;
    std::string world_view;
};


NS_BCX_END


FC_REFLECT( bcx::nh_asset_info,
           (id)
           (nh_hash)
           (base_describe)
           (nh_asset_creator)
           (nh_asset_owner)
           (nh_asset_active)
           (authority_account)
           (asset_qualifier)
           (world_view)
           )


