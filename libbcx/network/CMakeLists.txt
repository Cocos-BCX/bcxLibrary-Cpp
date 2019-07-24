
PROJECT( network )
CMAKE_MINIMUM_REQUIRED( VERSION 2.8.12 )

MESSAGE(STATUS "Configuring project network located in: ${CMAKE_CURRENT_SOURCE_DIR}")
SET( CMAKE_AUTOMOC OFF )

add_library( network
        STATIC
        Uri.cpp
        WebSocket.cpp)

target_include_directories(network
    PUBLIC
    ${THRID_LIB_DIR}/websockets/include/android
    ${THRID_LIB_DIR}/uv/include
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(network
    PUBLIC
    ${THRID_LIB_DIR}/websockets/prebuilt/android/${ANDROID_ABI}/libwebsockets.a
    ${THRID_LIB_DIR}/uv/prebuilt/android/${ANDROID_ABI}/libuv_a.a
    ${THRID_LIB_DIR}/zlib/prebuilt/android/${ANDROID_ABI}/libz.a
        ${OPENSSL_ROOT_DIR}/prebuilt/android/${ANDROID_ABI}/libssl.a
    ${OPENSSL_ROOT_DIR}/prebuilt/android/${ANDROID_ABI}/libcrypto.a

)


MESSAGE(STATUS "Finished network module configuration...")
