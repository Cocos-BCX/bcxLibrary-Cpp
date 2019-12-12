cmake_minimum_required(VERSION 3.6)
project(external)

add_library(external empty.cpp)

add_subdirectory(openssl)
add_subdirectory(secp256k1)
add_subdirectory(uv)
add_subdirectory(websockets)

target_link_libraries(external
    ext_secp256k1
    ext_uv
    ext_ssl
    ext_crypto
    ext_websockets
)

if(MACOSX OR ANDROID OR WINDOWS)
    add_subdirectory(zlib)
    target_link_libraries(external 
        ext_zlib
    )
endif()

# put "external" into External folder, too
# set_target_properties(external
#     PROPERTIES
#     ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
#     LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
#     FOLDER "External"
# )

