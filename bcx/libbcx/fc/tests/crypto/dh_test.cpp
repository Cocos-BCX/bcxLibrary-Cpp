#include <boost/test/unit_test.hpp>

#include <fc/crypto/dh.hpp>
#include <fc/exception/exception.hpp>

BOOST_AUTO_TEST_SUITE(fc_crypto)

BOOST_AUTO_TEST_CASE(dh_test)
{
    fc::diffie_hellman alice;
    BOOST_CHECK( alice.generate_params( 128, 5 ) );
    BOOST_CHECK( alice.generate_pub_key() );

    fc::diffie_hellman bob;
    bob.p = alice.p;
    BOOST_CHECK( bob.validate() );
    BOOST_CHECK( bob.generate_pub_key() );

    fc::diffie_hellman charlie;
    BOOST_CHECK( !charlie.validate() );
    BOOST_CHECK( !charlie.generate_pub_key() );
    charlie.p = alice.p;
    BOOST_CHECK( charlie.validate() );
    BOOST_CHECK( charlie.generate_pub_key() );

    BOOST_CHECK( alice.compute_shared_key( bob.pub_key ) );
    BOOST_CHECK( bob.compute_shared_key( alice.pub_key ) );
    BOOST_CHECK_EQUAL( alice.shared_key.size(), bob.shared_key.size() );
    BOOST_CHECK( !memcmp( alice.shared_key.data(), bob.shared_key.data(), alice.shared_key.size() ) );
    std::vector<char> alice_bob = alice.shared_key;

    BOOST_CHECK( alice.compute_shared_key( charlie.pub_key ) );
    BOOST_CHECK( charlie.compute_shared_key( alice.pub_key ) );
    BOOST_CHECK_EQUAL( alice.shared_key.size(), charlie.shared_key.size() );
    BOOST_CHECK( !memcmp( alice.shared_key.data(), charlie.shared_key.data(), alice.shared_key.size() ) );
    std::vector<char> alice_charlie = alice.shared_key;

    BOOST_CHECK( charlie.compute_shared_key( bob.pub_key ) );
    BOOST_CHECK( bob.compute_shared_key( charlie.pub_key ) );
    BOOST_CHECK_EQUAL( charlie.shared_key.size(), bob.shared_key.size() );
    BOOST_CHECK( !memcmp( charlie.shared_key.data(), bob.shared_key.data(), bob.shared_key.size() ) );
    std::vector<char> bob_charlie = charlie.shared_key;

    BOOST_CHECK( alice_bob.size() != alice_charlie.size() ||  memcmp( alice_bob.data(), alice_charlie.data(), alice_bob.size() ) );

    BOOST_CHECK( alice_bob.size() != bob_charlie.size() || memcmp( alice_bob.data(), bob_charlie.data(), alice_bob.size() ) );

    BOOST_CHECK( alice_charlie.size() != bob_charlie.size() || memcmp( alice_charlie.data(), bob_charlie.data(), alice_charlie.size() ) );

    alice.p.clear(); alice.p.push_back(100); alice.p.push_back(2);
    BOOST_CHECK( !alice.validate() );
    alice.p = bob.p;
    alice.g = 9;
    BOOST_CHECK( !alice.validate() );
}

static const std::string P(          "\344\005\357H\277<\305\356\006*\312\326\265\347;\363" );
static const std::string ALICE_PUB(  "\015\343p\243\344d\3600\015/UL\340\277\243\203" );
static const std::string ALICE_PRIV( "oJ'\363\227\243\346\215/V\253\036i\250\370\233" );
static const std::string BOB_PUB(    "26x`3\017A\365\334\022\031\231\032Y\242\242" );
static const std::string BOB_PRIV(   "d\300\327v?jG4\340\037k\221\230z\372\203" );
static const std::string SHARED_KEY( "\303.\236g\25767!\006\365u\341;o\241" );
BOOST_AUTO_TEST_CASE(dh_size_mismatch_test)
{
   fc::diffie_hellman alice;
   alice.p.insert( alice.p.begin(), P.begin(), P.end() );
   alice.pub_key.insert( alice.pub_key.begin(), ALICE_PUB.begin(), ALICE_PUB.end() );
   alice.priv_key.insert( alice.priv_key.begin(), ALICE_PRIV.begin(), ALICE_PRIV.end() );
   alice.g = 5;
   BOOST_CHECK( alice.validate() );

   fc::diffie_hellman bob;
   bob.p = alice.p;
   bob.pub_key.insert( bob.pub_key.begin(), BOB_PUB.begin(), BOB_PUB.end() );
   bob.priv_key.insert( bob.priv_key.begin(), BOB_PRIV.begin(), BOB_PRIV.end() );
   bob.g = alice.g;
   BOOST_CHECK( bob.validate() );

   BOOST_CHECK( alice.compute_shared_key( bob.pub_key ) );
   BOOST_CHECK( bob.compute_shared_key( alice.pub_key ) );
   BOOST_CHECK_EQUAL( 15, alice.shared_key.size() );
   BOOST_CHECK_EQUAL( 15, bob.shared_key.size() );
   BOOST_CHECK( !memcmp( alice.shared_key.data(), bob.shared_key.data(), alice.shared_key.size() ) );

   BOOST_CHECK_EQUAL( SHARED_KEY, std::string( alice.shared_key.begin(), alice.shared_key.end() ) );
}

BOOST_AUTO_TEST_SUITE_END()
