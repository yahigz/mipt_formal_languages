include_directories(${GTEST_INCLUDE_DIRS})

include_directories(${Testing_SOURCE_DIR}/include)

add_executable(testing_cyk cmake_test.cpp test_cyk.cpp test_cyk.hpp)

target_link_libraries(testing_cyk ${GTEST_LIBRARIES} grammar cyk tester Threads::Threads)
