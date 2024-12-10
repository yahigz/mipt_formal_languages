project(cyk)

set(cyk-source-files "${PROJECT_SOURCE_DIR}/cyk.cpp" CACHE INTERNAL "")

add_library(
    cyk SHARED ${cyk-source-files} ${Testing_SOURCE_DIR}/include/cyk.hpp
)

target_link_libraries(cyk grammar)
