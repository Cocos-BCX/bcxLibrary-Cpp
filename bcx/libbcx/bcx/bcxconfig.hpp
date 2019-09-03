//
//  bcxconfig.hpp
//  bcx
//
//  Created by hugo on 2019/4/9.
//  Copyright © 2019 SDKBox. All rights reserved.
//

#ifndef bcxconfig_hpp
#define bcxconfig_hpp

#include <string>
#include "../platform/PlatformMacros.h"

NS_BCX_BEGIN

struct bcxconfig {
    std::string wsnode;
    std::string faucet_url;
};

NS_BCX_END

#endif /* bcxconfig_hpp */
