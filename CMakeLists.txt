cmake_minimum_required(VERSION 2.6)
project(jsonip CXX)

set(CMAKE_CXX_FLAGS "-Wall")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_subdirectory(test)

