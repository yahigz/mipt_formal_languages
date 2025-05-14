project(tester)

set(tester-source-files "${PROJECT_SOURCE_DIR}/tester.cpp" CACHE INTERNAL "")

add_library(
    tester SHARED ${tester-source-files} ${Testing_SOURCE_DIR}/include/tester.hpp
)

target_link_libraries(tester cyk)
