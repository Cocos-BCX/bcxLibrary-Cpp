
#include "bcxtest.hpp"

#include "bcx/bcxlog.hpp"
#include "bcx/bcxproto.h"
#include "bcx/protocol/signed_block.hpp"
#include "bcx/protocol/contract_object.hpp"
#include "bcx/protocol/signed_block.hpp"

using namespace graphene;
using namespace graphene::chain;


// public
BCXTest::BCXTest() {
}

void BCXTest::init() {
    bcx::bcxapi& api = bcx::bcxapi::instance();

    _testMap = {
        {"1.0.login", [this, &api]() {
            if (api.isLogin()) {
                std::ostringstream os;
                os << "has login, ignore";
                this->showLog(os.str());
            } else {
                api.login("huang", "111111", [this](int error) {
                    std::ostringstream os;
                    os << "login:" << (0 == error ? "success" : "failed");
                    this->showLog(os.str());
                });
            }
        }},
        {"1.1.logout", [this, &api]() {
            std::ostringstream os;
            if (api.isLogin()) {
                api.logout();
                os << "logout success";
                this->showLog(os.str());
            } else {
                os << "has logout, ignore";
                this->showLog(os.str());
            }
        }},
        {"1.2.create account", [this, &api]() {
            api.createAccount("sdkbox", "111111", [this](bool suc, const std::string& error) {
                std::ostringstream os;
                os << "create account:"<< suc << "," << error.c_str();
                this->showLog(os.str());
            });
        }},
        {"1.3.upgrade account", [this, &api]() {
            api.upgradeAccount([this](int rst) {
                std::ostringstream os;
                os << "upgrade account:"<< (0 == rst ? "suc" : "failed");
                this->showLog(os.str());
            });
        }},
        {"1.4.get full account", [this, &api]() {
            api.get_full_accounts("hugoo", [=](const graphene::chain::account_object &var) {
                std::ostringstream os;
                os << "full account:" << var.get_id().instance.value;
                this->showLog(os.str());
            });
        }},
        {"1.5.get key references", [this, &api]() {
            api.get_key_references([this](const std::string &id) {
                std::ostringstream os;
                os << "key references:" << id;
                this->showLog(os.str());
            });
        }},
        {"1.6.get chain id", [this, &api]() {
            api.get_chain_id([this](std::string chain_id) {
                std::ostringstream os;
                os << "chain id :" << chain_id;
                this->showLog(os.str());
            });
        }},
        {"2.0.transfer", [this, &api]() {
            api.transfer("hugoo", "COCOS", /*3*100000*/ 33, "COCOS", "", [this](int error, const std::string &trxid) {
                std::ostringstream os;
                os << "transfer:" << (0 == error ? "success" : "failed") << " trxID:" << trxid;
                this->showLog(os.str());
            });
        }},
        {"2.1.look up symbol", [this, &api]() {
            api.lookup_asset_symbols({"COCOS"}, [this](const std::vector<graphene::chain::asset_object> &rst) {
                std::ostringstream os;
                os << "Asset:" << rst.size();
                for(auto it : rst) {
                    os << " | " << it.symbol;
                }
                this->showLog(os.str());
            });
        }},
        {"2.2.look up block", [this, &api]() {
            api.get_block(793927, [this](const fc::optional<bcx::protocol::signed_block>& sb) {
                std::ostringstream os;
                if (sb.valid()) {
                    os << "block:" << (*sb).block_num();
                }
                this->showLog(os.str());
            });
        }},
        {"2.3.list assets", [this, &api]() {
            api.list_assets("A", 10, [this](const std::vector<graphene::chain::asset_object>& assets, int error) {
                std::ostringstream os;
                os << "list_assets:";
                for (const auto& asset : assets) {
                    os << asset.symbol << " ";
                }
                this->showLog(os.str());
            });
        }},
        {"2.4.create asset", [this, &api]() {
            api.create_asset("SDKBOX", 100, 4, 1, "a test asset", [this](int error) {
                std::ostringstream os;
                os << "create_asset:" << error;
                this->showLog(os.str());
            });
        }},
        {"2.5.update asset", [this, &api]() {
            api.update_asset("SDKBOX", 10000000, 2, "a test asset1", [this](int error) {
                std::ostringstream os;
                os << "update_asset:" << error;
                this->showLog(os.str());
            });
        }},
        {"3.0.get_contract", [this, &api]() {
            //bcx::bcxapi::instance().get_contract("1.16.2");
            std::ostringstream os;

            api.get_contract("contract.sdkboxtest01", [this](int error, const bcx::protocol::contract_object& co) {
                std::ostringstream os;
                os << "contract object:" << co.name;
                this->showLog(os.str());
            });
        }},
        {"3.1.create_contract", [this, &api]() {
            std::string contract_source = "\
function set_public_data(key, val) \
    public_data[key] = val \
\
    write_list={public_data={[key]=true}} \
    chainhelper:write_chain() \
end \
\
function get_public_data(key) \
    read_list={public_data={[key]=true}} \
    chainhelper:read_chain() \
\
    chainhelper:log(public_data[key]) \
end \
\
function set_private_data(key, val) \
    private_data[key] = val \
\
    write_list={private_data={[key]=true}} \
    chainhelper:write_chain() \
end \
\
function get_private_data(key) \
    read_list={private_data={[key]=true}} \
    chainhelper:read_chain() \
\
    chainhelper:log(private_data[key]) \
end \
\
function dump_params(param1, param2) \
    chainhelper:log('param1:' .. cjson.encode(param1) .. ' param2:' .. param2) \
end \
\
function set_permissions_flag(flag) \
    assert(is_owner(), \"You are not owner\") \
\
    log('set permissions finish'); \
end";
            api.create_contract("contract.sdkboxtest01", contract_source, [this](int error) {
                std::ostringstream os;
                os << "create_contract:" << error;
                this->showLog(os.str());
            });
        }},
        {"3.2.update_contract", [this, &api]() {
            std::string contract_source = "\
function set_public_data(key, val) \
    public_data[key] = val \
\
    write_list={public_data={[key]=true}} \
    chainhelper:write_chain() \
end \
\
function get_public_data(key) \
    read_list={public_data={[key]=true}} \
    chainhelper:read_chain() \
\
    chainhelper:log(public_data[key]) \
end \
\
function set_private_data(key, val) \
    private_data[key] = val \
\
    write_list={private_data={[key]=true}} \
    chainhelper:write_chain() \
end \
\
function get_private_data(key) \
    read_list={private_data={[key]=true}} \
    chainhelper:read_chain() \
\
    chainhelper:log(private_data[key]) \
end \
\
function dump_params(param1, param2) \
    chainhelper:log('param1:' .. cjson.encode(param1) .. ' param2:' .. param2) \
end \
\
function set_permissions_flag(flag) \
    assert(is_owner(), \"You are not owner\") \
\
    log('set permissions finish'); \
end";

            api.update_contract("contract.sdkboxtest01", contract_source, [this](int error) {
                std::ostringstream os;
                os << "update contract:" << error;
                this->showLog(os.str());
            });
        }},
        {"3.3.call_contract_function", [this, &api]() {
            std::string funName = "dump_params";
            std::vector<bcx::lua_types> params;

            //param1
            bcx::lua_map map;
            map[bcx::lua_key(bcx::lua_string("string"))] = bcx::lua_string("3331");
            map[bcx::lua_key(bcx::lua_string("int"))] = bcx::lua_int(2);
            map[bcx::lua_key(bcx::lua_string("bool"))] = bcx::lua_bool(true);
            map[bcx::lua_key(bcx::lua_string("number"))] = bcx::lua_number(3.1415);
            params.push_back(bcx::lua_table(map));

            //param2
            params.push_back(bcx::lua_string("sdkbox1"));
            
//            funName = "set_public_data";
//            params.push_back(bcx::lua_string("k1"));
//            params.push_back(bcx::lua_string("v1"));

            api.call_contract_function("contract.sdkboxtest01", funName, params, 5, [this](int error) {
                std::ostringstream os;
                os << "call_contract_function:" << error;
                this->showLog(os.str());
            });
        }},
        {"4.0.NH register creator", [this, &api]() {
            api.nh_register_creator([this](int error) {
                std::ostringstream os;
                os << "nh_register_creator:" << error;
                this->showLog(os.str());
            });
        }},
        {"4.1.NH create world view", [this, &api]() {
            api.nh_create_world_view("SDKBOX", [this](int error) {
                std::ostringstream os;
                os << "nh_create_world_view:" << error;
                this->showLog(os.str());
            });
        }},
        {"4.2.NH relate world view", [this, &api]() {
            api.nh_relate_world_view("SDKBOX", [this](int error) {
                std::ostringstream os;
                os << "nh_relate_world_view:" << error;
                this->showLog(os.str());
            });
        }},
        {"4.3.NH create asset", [this, &api]() {
            std::vector<bcx::nh_asset_create_info> acis;
        
            bcx::nh_asset_create_info aci;
            aci.asset_name = "COCOS";
            aci.base_describe = "{\"name\":\"Knife\"}";
            aci.world_view = "SDKBOX";
            aci.owner = "hugoo";
        
            acis.push_back(aci);
            api.nh_creat_asset(acis, [this](int error, std::string s) {
                std::ostringstream os;
                os << "nh_creat_asset:" << error << " " << s;
                this->showLog(os.str());
            });
        }},
        {"4.4.NH delete asset", [this, &api]() {
            api.nh_delete_asset({"4.2.79"}, [this](int error) {
                std::ostringstream os;
                os << "nh_delete_asset:" << error;
                this->showLog(os.str());
            });
        }},
        {"4.5.NH transfer asset", [this, &api]() {
            api.nh_transfer_asset("4.2.79", "hugoo", [this](int error) {
                std::ostringstream os;
                os << "nh_transfer_asset:" << error;
                this->showLog(os.str());
            });
        }},
        {"4.6.NH lookup asset", [this, &api]() {
            api.nh_lookup_assets({"4.2.77", "75ad3e628d1da20ca5f7d65c7fde0867e5b7b827fe1aa9b4b3fc76b94ea08507"},
                                 [this](int error, const std::vector<fc::optional<bcx::nh_asset_info>>& assets) {
                std::ostringstream os;
                os << "nh_lookup_assets:" << error;
                this->showLog(os.str());
                for (const auto& ai : assets) {
                    if (ai.valid()) {
                        os.clear();
                        os.str("");
                        os << "asset id:" << ai->id.instance.value
                            << " hash:" << ai->nh_hash.data()
                            << " world:" << ai->world_view.data()
                            << " descript:" << ai->base_describe.data();
                        this->showLog(os.str());
                    } else {
                        os.clear();
                        os.str("");
                        os << "asset is invalid";
                        this->showLog(os.str());
                    }
                }
            });
        }}
    };
    
    bcx::bcxconfig cfg;
    cfg.wsnode = "ws://test.cocosbcx.net";
    cfg.faucet_url = "http://test-faucet.cocosbcx.net";
    bcx::bcxapi::instance().init(cfg, [this](int error) {
        std::ostringstream os;
        os << "bcx init:"<< (0 == error ? "success" : "failed");
        this->showLog(os.str());
    });
}


void BCXTest::poolEvent() {
    bcx::bcxapi::instance().poolEvent();
}

void BCXTest::onBtnLoginOrLogout() {
    bcx::bcxapi& api = bcx::bcxapi::instance();
    if (api.isLogin()) {
        api.logout();
        this->showLog("logout");
    } else {
        api.login("huang", "111111", [this](int error) {
            std::ostringstream os;
            os << "login:" << (0 == error ? "success" : "failed");
            this->showLog(os.str());
        });
    }
}

void BCXTest::onBtnTest() {
    bcx::bcxapi& api = bcx::bcxapi::instance();


//    _api.get_objects({"2.1.0"}, [](const fc::variants& cb) {
//        bcx::log("get_objects success");
//    });
//     _api.get_account_history("1.2.21", "1.11.0", 1, "1.11.0",
//                              [](const std::vector<graphene::chain::operation_history_object>&  history) {
//         bcx::log("operation history object");
//     });




}

void BCXTest::test() {
    graphene::chain::signed_transaction trx;
    {

        auto seed = account + "owner" + pwd;
        graphene::chain::account_id_type nathan_id;
        fc::ecc::private_key nathan_key = fc::ecc::private_key::regenerate(fc::sha256::hash(fc::string(seed)));

//        graphene::chain::balance_claim_operation claim_op;
//        graphene::chain::balance_id_type bid = graphene::chain::balance_id_type();
//        claim_op.deposit_to_account = nathan_id;
//        claim_op.balance_to_claim = bid;
//        claim_op.balance_owner_key = nathan_key.get_public_key();
//        // claim_op.total_claimed = bid(*db1).balance;
//        trx.operations.push_back( claim_op );
//        // db1->current_fee_schedule().set_fee( trx.operations.back() );

        graphene::chain::transfer_operation xfer_op;
        xfer_op.from = object_id_type(1,2,21);
        xfer_op.to = object_id_type(1,2,22);
        xfer_op.amount = graphene::chain::asset( 1000000 );

//        xfer_op.fee.amount = 200000;
        //        xfer_op.fee.asset_id = "1.3.0"; ??

        trx.operations.push_back( xfer_op );

        trx.ref_block_num = 11084;
        trx.ref_block_prefix = 71151825;



        // db1->current_fee_schedule().set_fee( trx.operations.back() );




        // trx.set_expiration( db1->get_slot_time( 10 ) );
        fc::time_point_sec t = fc::time_point::now();
        t += 10;
//        t = fc::time_point_sec::from_iso_string(fc::string("2019-03-05T15:35:16"));
        trx.set_expiration(t);


        const graphene::chain::signed_transaction xtrx(trx);
//
//        //        trx.sign( nathan_key, db1->get_chain_id() );
        const graphene::chain::chain_id_type chain_id("");
        const auto& x = xtrx.sign(nathan_key, chain_id);
        trx.signatures.push_back(x);

//        trx.sign(nathan_key, chain_id);
        trx.validate();
    }

    fc::variants params;
    params.push_back(3);
    params.push_back("broadcast_transaction_with_callback");
    fc::variants trxList;
    trxList.push_back(fc::variant(trx, (uint32_t)100));
    params.push_back(trxList);
    int counter = 0;
    fc::rpc::request request{++counter, "call", params};
    std::string s = fc::json::to_string(fc::variant(request, (uint32_t)100), fc::json::stringify_large_ints_and_doubles, 100);
    BCXLOG("trans> %s", s.c_str());
}

void BCXTest::send(const std::string& data)
{
    bcx::bcxapi::instance().send(data);
}

std::vector<std::string> BCXTest::functionList() {
    
    std::vector<std::string> keys;
    keys.reserve(_testMap.size());
    for(auto kv : _testMap) {
        keys.push_back(kv.first);
    }

    return keys;
}

void BCXTest::exec(const std::string& funName) {
    auto it = _testMap.find(funName);
    if (it == _testMap.end()) return ;
    if (it->second == nullptr) return ;
    
    it->second();
}
