cmake_minimum_required (VERSION 3.21.1)

if(DEFINED ENV{IDF_PATH} AND NOT QUL_PLATFORM STREQUAL "qt")
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
endif()

project(edge_coffe_machine VERSION 0.0.1 LANGUAGES C CXX ASM)

find_package(Qul QUIET)

add_subdirectory(qmlproject)
add_subdirectory(tests/logic_tests)