cmake_minimum_required(VERSION 3.12)

project(asteroids)

set(TIGR_DIR $ENV{PROJECTS_PATH}/tigr)

set(CMAKE_BUILD_TYPE Release)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXE_LINKER_FLAGS "-framework OpenGL -framework Cocoa")

include_directories(${CMAKE_SOURCE_DIR} $ENV{PROJECTS_PATH}/tigr)

add_executable(test_astobj main.C ast_obj.C display.C ${TIGR_DIR}/tigr.C)

