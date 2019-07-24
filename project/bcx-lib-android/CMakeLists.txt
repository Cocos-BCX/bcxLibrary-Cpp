# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

SET(BOOST_COMPONENTS)
#LIST(APPEND BOOST_COMPONENTS thread date_time system filesystem program_options signals serialization chrono unit_test_framework context locale iostreams)
LIST(APPEND BOOST_COMPONENTS thread date_time system filesystem chrono)
#set(Boost_INCLUDE_DIRS ${BOOST_ROOT}/${ANDROID_ABI}/include/)

set(Boost_INCLUDE_DIRS ${BOOST_ROOT}/include/)
set(Boost_LIBRARY_DIRS ${BOOST_ROOT}/lib/${ANDROID_ABI})
set(Boost_LIBRARIES $BOOST_COMPONENTS)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${OPENSSL_ROOT_DIR}/include/android)
include_directories(${THRID_LIB_DIR}/secp256k1/include)
include_directories(${THRID_LIB_DIR}/promise-cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../libbcx/fc/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../libbcx/db/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../libbcx/chain/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../test/Classes)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../libbcx/)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        bcx

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        src/main/cpp/native-lib.cpp
        ../../../test/Classes/bcxtest.cpp
        )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        bcx

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})

add_subdirectory( ../../libbcx
        libbcx)

add_library(libcrypto STATIC IMPORTED )
add_library(libssl STATIC IMPORTED )

set_target_properties(libssl PROPERTIES IMPORTED_LOCATION ${OPENSSL_ROOT_DIR}/prebuilt/android/${ANDROID_ABI}/libssl.a)
set_target_properties(libcrypto PROPERTIES IMPORTED_LOCATION ${OPENSSL_ROOT_DIR}/prebuilt/android/${ANDROID_ABI}/libcrypto.a)

target_link_libraries(bcx
        libssl
        libcrypto
        bcxapi
        fc
)


