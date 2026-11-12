file(GLOB_RECURSE ITCH_TEST_SOURCES CONFIGURE_DEPENDS "*.cc")

if(NOT ITCH_TEST_SOURCES)
    message(STATUS "tests/: no sources yet, skipping itch_tests")
    return()
endif()

find_package(Catch2 3 CONFIG REQUIRED)

add_executable(itch_tests ${ITCH_TEST_SOURCES})

target_link_libraries(itch_tests
    PRIVATE
        nasdaq_itch::itch
        Catch2::Catch2WithMain
)

add_test(NAME itch_tests COMMAND itch_tests)
