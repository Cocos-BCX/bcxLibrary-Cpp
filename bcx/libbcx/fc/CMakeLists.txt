#
# Defines fc library target.

# PROJECT( fc )
CMAKE_MINIMUM_REQUIRED( VERSION 2.8.12 )

MESSAGE(STATUS "Configuring project fc located in: ${CMAKE_CURRENT_SOURCE_DIR}")
SET( CMAKE_AUTOMOC OFF )

# Setup module path to make visible used CMake extensions
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/GitVersionGen")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")


SET( ECC_IMPL secp256k1 CACHE STRING "secp256k1 or openssl or mixed" )

IF( ECC_IMPL STREQUAL openssl )
    SET( ECC_REST src/crypto/elliptic_impl_pub.cpp )
ELSE( ECC_IMPL STREQUAL openssl )
    SET( ECC_LIB secp256k1 )
    IF( ECC_IMPL STREQUAL mixed )
        SET( ECC_REST src/crypto/elliptic_impl_priv.cpp src/crypto/elliptic_impl_pub.cpp )
    ELSE( ECC_IMPL STREQUAL mixed )
        SET( ECC_REST src/crypto/elliptic_impl_priv.cpp )
    ENDIF( ECC_IMPL STREQUAL mixed )
ENDIF( ECC_IMPL STREQUAL openssl )


SET(BOOST_COMPONENTS)
#LIST(APPEND BOOST_COMPONENTS thread date_time system filesystem program_options signals serialization chrono unit_test_framework context iostreams)
LIST(APPEND BOOST_COMPONENTS thread date_time system filesystem chrono)

#SET(Boost_LIBRARIES "")
#foreach(COMPONENT ${BOOST_COMPONENTS})
#add_library(BOOST_LIB_${COMPONENT} STATIC IMPORTED )
#set_target_properties(BOOST_LIB_${COMPONENT} PROPERTIES IMPORTED_LOCATION
#        ${BOOST_ROOT}/lib/${ANDROID_ABI}/libboost_${COMPONENT}-clang-darwin-mt-1_65_1.a)
#LIST(APPEND Boost_LIBRARIES BOOST_LIB_${COMPONENT})
#endforeach()

# libboost.a
add_library(boost STATIC IMPORTED )
set_target_properties(
    boost
    PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../3rd/boost/android/lib/${ANDROID_ABI}/libboost.a)


set( fc_sources
     src/uint128.cpp
     src/real128.cpp
     src/variant.cpp
     src/exception.cpp
     src/variant_object.cpp
     src/thread/thread.cpp
     src/thread/thread_specific.cpp
     src/thread/future.cpp
     src/thread/task.cpp
     src/thread/spin_lock.cpp
     src/thread/spin_yield_lock.cpp
     src/thread/mutex.cpp
     src/thread/non_preemptable_scope_check.cpp
     src/asio.cpp
     src/string.cpp
     src/shared_ptr.cpp
     src/stacktrace.cpp
     src/time.cpp
     src/utf8.cpp
     src/io/iostream.cpp
     src/io/datastream.cpp
     src/io/buffered_iostream.cpp
     src/io/fstream.cpp
     src/io/sstream.cpp
     src/io/json.cpp
     src/io/varint.cpp
     src/io/console.cpp
     src/filesystem.cpp
     src/interprocess/signals.cpp
     src/interprocess/file_mapping.cpp
     src/interprocess/mmap_struct.cpp
     src/interprocess/file_mutex.cpp
     src/rpc/cli.cpp
     src/rpc/http_api.cpp
     src/rpc/json_connection.cpp
     src/rpc/state.cpp
     src/rpc/bstate.cpp
     src/rpc/websocket_api.cpp
     src/log/log_message.cpp
     src/log/logger.cpp
     src/log/appender.cpp
     src/log/console_appender.cpp
     src/log/file_appender.cpp
     src/log/gelf_appender.cpp
     src/log/logger_config.cpp
     src/crypto/_digest_common.cpp
     src/crypto/openssl.cpp
     src/crypto/aes.cpp
     src/crypto/crc.cpp
     src/crypto/city.cpp
     src/crypto/base36.cpp
     src/crypto/base58.cpp
     src/crypto/base64.cpp
     src/crypto/bigint.cpp
     src/crypto/hex.cpp
     src/crypto/sha1.cpp
     src/crypto/ripemd160.cpp
     src/crypto/sha256.cpp
     src/crypto/sha224.cpp
     src/crypto/sha512.cpp
     src/crypto/dh.cpp
     src/crypto/blowfish.cpp
     src/crypto/elliptic_common.cpp
     ${ECC_REST}
     src/crypto/elliptic_${ECC_IMPL}.cpp
     src/crypto/rand.cpp
     src/compress/zlib.cpp
     )

file( GLOB_RECURSE fc_headers ${CMAKE_CURRENT_SOURCE_DIR} *.hpp *.h )

set( sources
  ${fc_sources}
)

add_library(fc
        STATIC
        ${sources})

# include_directories(${Boost_INCLUDE_DIRS})

target_include_directories(fc
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)


add_library(secp256k1 STATIC IMPORTED )
set_target_properties(secp256k1 PROPERTIES IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/../3rd/secp256k1/prebuilt/android/${ANDROID_ABI}/libsecp256k1.a)

target_link_libraries(fc
        ${LINK_USR_LOCAL_LIB}
        secp256k1
        boost
        #${Boost_LIBRARIES}
)

MESSAGE(STATUS "Finished fc module configuration...")
