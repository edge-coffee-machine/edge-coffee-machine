cmake_minimum_required(VERSION 3.16)
project(edge_coffe_machine_unit_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
enable_testing()

# Build an executable that compiles the Beverage implementation together with the test.
add_executable(unit_tests
    main.cpp
    TestBeverage.cpp
    TestUser.cpp
    TestWeightedSortedList.cpp
    ../qmlproject/logic/Beverage.cpp
    ../qmlproject/logic/User.cpp
    ../qmlproject/logic/WeightedSortedList.h
)

target_include_directories(unit_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/tests/include
    ${CMAKE_SOURCE_DIR}/qmlproject/logic
    ${CMAKE_SOURCE_DIR}/tests
)

target_compile_features(unit_tests PRIVATE cxx_std_17)

add_test(NAME unit_tests COMMAND unit_tests)
