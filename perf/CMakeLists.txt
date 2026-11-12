file(GLOB_RECURSE ITCH_PERF_SOURCES CONFIGURE_DEPENDS "*.cc")

if(NOT ITCH_PERF_SOURCES)
    message(STATUS "perf/: no sources yet, skipping itch_perf")
    return()
endif()

find_package(benchmark CONFIG REQUIRED)

add_executable(itch_perf ${ITCH_PERF_SOURCES})

target_link_libraries(itch_perf
    PRIVATE
        nasdaq_itch::itch
        benchmark::benchmark
        benchmark::benchmark_main
)
