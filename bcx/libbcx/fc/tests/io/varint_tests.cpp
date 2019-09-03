#include <boost/test/unit_test.hpp>

#include <fc/io/json.hpp>
#include <fc/io/raw.hpp>
#include <fc/io/varint.hpp>

BOOST_AUTO_TEST_SUITE(varint_tests)

#define UINT_LENGTH (1 + 5*1 + 3*2 + 3*3 + 3*4 + 2*5 + 3*6 + 3*7 + 3*8 + 3*9 + 2*10)
static const std::string EXPECTED_UINTS( "\036" // 30 = length of array
                                         "\0\1\2\020\177"
                                         "\200\1\200\2\377\177"
                                         "\200\200\1\200\200\2\377\377\177"
                                         "\200\200\200\1\200\200\200\2\377\377\377\177"
                                         "\200\200\200\200\1"
                                         "\252\325\252\325\012"
                                         "\200\200\200\200\200\1"
                                         "\200\200\200\200\200\2"
                                         "\377\377\377\377\377\177"
                                         "\200\200\200\200\200\200\1"
                                         "\200\200\200\200\200\200\2"
                                         "\377\377\377\377\377\377\177"
                                         "\200\200\200\200\200\200\200\1"
                                         "\200\200\200\200\200\200\200\2"
                                         "\377\377\377\377\377\377\377\177"
                                         "\200\200\200\200\200\200\200\200\1"
                                         "\200\200\200\200\200\200\200\200\2"
                                         "\377\377\377\377\377\377\377\377\177"
                                         "\200\200\200\200\200\200\200\200\200\1"
                                         "\377\377\377\377\377\377\377\377\377\1",
                                         UINT_LENGTH );
static const std::vector<fc::unsigned_int> TEST_U = {
                                                 0, // \0
                                                 1, // \1
                                                 2, // \2
                                              0x10, // \020
                                              0x7f, // \177
                                              0x80, // \200\1
                                             0x100, // \200\2
                                            0x3fff, // \377\177
                                            0x4000, // \200\200\1
                                            0x8000, // \200\200\2
                                          0x1fffff, // \377\377\177
                                          0x200000, // \200\200\200\1
                                          0x400000, // \200\200\200\2
                                         0xfffffff, // \377\377\377\177
                                        0x10000000, // \200\200\200\200\1
                                        0xaaaaaaaa, // \252\325\252\325\012
                                       0x800000000ULL, // \200\200\200\200\200\1
                                      0x1000000000ULL, // \200\200\200\200\200\2
                                     0x3ffffffffffULL, // \377\377\377\377\377\177
                                     0x40000000000ULL, // \200\200\200\200\200\200\1
                                     0x80000000000ULL, // \200\200\200\200\200\200\2
                                   0x1ffffffffffffULL, // \377\377\377\377\377\377\177
                                   0x2000000000000ULL, // \200\200\200\200\200\200\200\1
                                   0x4000000000000ULL, // \200\200\200\200\200\200\200\2
                                  0xffffffffffffffULL, // \377\377\377\377\377\377\377\177
                                 0x100000000000000ULL, // \200\200\200\200\200\200\200\200\1
                                 0x200000000000000ULL, // \200\200\200\200\200\200\200\200\2
                                0x7fffffffffffffffULL, // \377\377\377\377\377\377\377\377\177
                                0x8000000000000000ULL, // \200\200\200\200\200\200\200\200\200\1
                                0xffffffffffffffffULL  // \377\377\377\377\377\377\377\377\377\1
   };
BOOST_AUTO_TEST_CASE( test_unsigned )
{ try {
   const std::vector<char> packed_u = fc::raw::pack<std::vector<fc::unsigned_int>>( TEST_U, 3 );
   BOOST_CHECK_EQUAL( UINT_LENGTH, packed_u.size() );
   BOOST_CHECK_EQUAL( EXPECTED_UINTS, std::string( packed_u.data(), packed_u.size() ) );
   std::vector<fc::unsigned_int> unpacked_u;
   fc::raw::unpack<std::vector<fc::unsigned_int>>( packed_u, unpacked_u, 3 );
   BOOST_CHECK_EQUAL( TEST_U.size(), unpacked_u.size() );
   for( size_t i = 0; i < TEST_U.size(); i++ )
      BOOST_CHECK_EQUAL( TEST_U[i].value, unpacked_u[i].value );

   const std::string json_u = fc::json::to_string(fc::variant( TEST_U, 3 ));
   BOOST_CHECK_EQUAL( "[0,1,2,16,127,128,256,16383,16384,32768,2097151,2097152,4194304,268435455,268435456,2863311530,"
                       "\"34359738368\",\"68719476736\",\"4398046511103\",\"4398046511104\","
                       "\"8796093022208\",\"562949953421311\",\"562949953421312\","
                       "\"1125899906842624\",\"72057594037927935\",\"72057594037927936\","
                       "\"144115188075855872\",\"9223372036854775807\",\"9223372036854775808\","
                       "\"18446744073709551615\"]", json_u );
   std::vector<fc::unsigned_int> unjson_u = fc::json::from_string( json_u ).as<std::vector<fc::unsigned_int>>( 3 );
   BOOST_CHECK_EQUAL( TEST_U.size(), unjson_u.size() );
   for( size_t i = 0; i < TEST_U.size(); i++ )
      BOOST_CHECK_EQUAL( TEST_U[i].value, unjson_u[i].value );
} FC_LOG_AND_RETHROW() }

BOOST_AUTO_TEST_CASE( test_limits )
{ try {
   static const std::string overflow = "\200\200\200\200\200\200\200\200\200\2"; // = 2^64
   static const std::string overlong = "\200\200\200\200\200\200\200\200\300\200\1";

   fc::unsigned_int dest;
   BOOST_CHECK_THROW( fc::raw::unpack( std::vector<char>( overflow.begin(), overflow.end() ), dest, 3 ), fc::overflow_exception );
   BOOST_CHECK_THROW( fc::raw::unpack( std::vector<char>( overlong.begin(), overlong.end() ), dest, 3 ), fc::overflow_exception );
} FC_LOG_AND_RETHROW() }

BOOST_AUTO_TEST_SUITE_END()
