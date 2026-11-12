#include "itch/sentinel.hh"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("sentinel links against itch library", "[sentinel]") {
    REQUIRE(itch::sentinel_value() == 42);
    REQUIRE(itch::library_name() == "nasdaq_itch");
}
