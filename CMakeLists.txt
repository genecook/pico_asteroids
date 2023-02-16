cmake_minimum_required(VERSION 3.12)

project(asteroids)

set(CMAKE_BUILD_TYPE Release)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(test_astobj main.C ast_obj.C)

