cmake_minimum_required(VERSION 3.23)

project(nasdaq_itch VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ITCH_BUILD_PERF "Build Google Benchmark perf targets" ON)

include(CTest)

# Adds every immediate subdirectory of the current source dir that has its own
# CMakeLists.txt. Used by exec/ and sandbox/ so new subprojects are picked up
# without touching the parent list.
function(itch_add_subprojects)
    file(GLOB _entries LIST_DIRECTORIES true "${CMAKE_CURRENT_SOURCE_DIR}/*")
    list(SORT _entries)
    foreach(_entry IN LISTS _entries)
        if(IS_DIRECTORY "${_entry}" AND EXISTS "${_entry}/CMakeLists.txt")
            add_subdirectory("${_entry}")
        endif()
    endforeach()
endfunction()

add_subdirectory(lib)
add_subdirectory(exec)
add_subdirectory(sandbox)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

if(ITCH_BUILD_PERF)
    add_subdirectory(perf)
endif()
