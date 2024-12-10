cmake_minimum_required(VERSION 3.8)
project(Testing)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" ${CMAKE_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${Testing_SOURCE_DIR}/bin)


find_package(GTest REQUIRED)
find_package(GMock REQUIRED)
find_package(Threads REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})
include_directories(${GMOCK_INCLUDE_DIRS})

include_directories(include)

add_subdirectory(src/grammar)
add_subdirectory(src/cocke_younger_kasami)
add_subdirectory(src/tester)

add_subdirectory(testing)

enable_testing()

add_test(Test ${Testing_SOURCE_DIR}/bin/testing_cyk)
