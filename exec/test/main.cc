#include "itch/sentinel.hh"

#include <print>

int main() {
    std::println("{} sentinel: {}", itch::library_name(), itch::sentinel_value());
    return itch::sentinel_value() == 42 ? 0 : 1;
}
