#pragma once

#include "./types.hpp"

#pragma once

namespace bcx { namespace protocol {

class contract_object : public abstract_object<contract_object> {
public:
    fc::time_point_sec creation_date;
    account_id_type owner;
    std::string name;
    std::string current_version;
    std::string contract_authority;
    bool check_contract_authority;
    std::string lua_code;
};

} }

FC_REFLECT_DERIVED( bcx::protocol::contract_object, (bcx::protocol::object),
                   (creation_date)(owner)(name)(current_version)(contract_authority)(check_contract_authority)(lua_code)
)

