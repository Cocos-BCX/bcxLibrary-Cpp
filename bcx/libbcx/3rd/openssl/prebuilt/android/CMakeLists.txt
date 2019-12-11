
cmake_minimum_required(VERSION 3.6)

set(lib_name crypto)
set(target_name ext_${crypto})

project(${lib_name})

#include(../../../../cocos2d/external/cmake/CocosExternalConfig.cmake)

add_library(${target_name} STATIC IMPORTED GLOBAL)

set_target_properties(${target_name} PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/../../include/android"
)
if(ANDROID)
  if("${ANDROID_STL}" STREQUAL "c++_static")
      set(SDKBOX_LIBS_FOLDER libs_c++_static)
  else()
      set(SDKBOX_LIBS_FOLDER libs)
  endif()
  set_target_properties(${target_name} PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/${SDKBOX_LIBS_FOLDER}/${ANDROID_ABI}/lib${lib_name}.a"
  )
endif()

set(lib_name ssl)
set(target_name ext_${ssl})

project(${lib_name})

#include(../../../../cocos2d/external/cmake/CocosExternalConfig.cmake)

add_library(${target_name} STATIC IMPORTED GLOBAL)

set_target_properties(${target_name} PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/../../include/android"
)
if(ANDROID)
  if("${ANDROID_STL}" STREQUAL "c++_static")
      set(SDKBOX_LIBS_FOLDER libs_c++_static)
  else()
      set(SDKBOX_LIBS_FOLDER libs)
  endif()
  set_target_properties(${target_name} PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/${SDKBOX_LIBS_FOLDER}/${ANDROID_ABI}/lib${lib_name}.a"
  )
endif()
