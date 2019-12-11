
cmake_minimum_required(VERSION 3.4.1)

PROJECT( bcx )

MESSAGE(STATUS "Configuring project bcx_lib located in: ${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE BCX_SOURCES "bcx/*.cpp" "fc/*.cpp" "log/*.cpp" "network/*.cpp" "platform/*.cpp" "protocol/*.cpp" "utils/*.cpp")
file(GLOB_RECURSE BCX_HEADERS "bcx/*.hpp" "fc/*.hpp" "log/*.hpp" "network/*.hpp" "platform/*.hpp" "protocol/*.hpp" "utils/*.hpp")

add_library(bcx STATIC
    ${BCX_SOURCES}
    ${BCX_HEADERS})

# ndk log
find_library(log-lib log)

target_include_directories(bcx
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd/openssl/include/android
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd/secp256k1/include
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd/boost/android/include
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd/uv/include
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd/websockets/include/android
    ${CMAKE_CURRENT_SOURCE_DIR}/fc/include
)

target_link_libraries(bcx
    PUBLIC
    ${log-lib}
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd/zlib/prebuilt/android/${ANDROID_ABI}/libz.a
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd/openssl/prebuilt/android/${ANDROID_ABI}/libssl.a
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd/openssl/prebuilt/android/${ANDROID_ABI}/libcrypto.a
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd/secp256k1/prebuilt/android/${ANDROID_ABI}/libsecp256k1.a
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd/uv/prebuilt/android/${ANDROID_ABI}/libuv_a.a
    ${CMAKE_CURRENT_SOURCE_DIR}/3rd/websockets/prebuilt/android/${ANDROID_ABI}/libwebsockets.a
)

MESSAGE(STATUS "Finished bcx_lib module configuration...")
