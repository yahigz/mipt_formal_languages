project(grammar)

set(grammar-source-files "${PROJECT_SOURCE_DIR}/grammar.cpp" CACHE INTERNAL "")

add_library(
    grammar SHARED ${grammar-source-files} ${Testing_SOURCE_DIR}/include/grammar.hpp
)