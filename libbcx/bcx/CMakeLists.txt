
cmake_minimum_required(VERSION 3.4.1)
PROJECT( bcxapi )

MESSAGE(STATUS "Configuring project bcxapi located in: ${CMAKE_CURRENT_SOURCE_DIR}")
SET( CMAKE_AUTOMOC OFF )

file(GLOB_RECURSE BCX_SOURCES . *.cpp)
file(GLOB_RECURSE BCX_HEADERS . *.h *.hpp)

MESSAGE(STATUS BCX_HEADERS)

add_library(bcxapi
        STATIC
        ${BCX_SOURCES}
        ${BCX_HEADERS})

target_include_directories( bcxapi
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(bcxapi network graphene_chain fc graphene_db)


MESSAGE(STATUS "Finished bcxapi module configuration...")

