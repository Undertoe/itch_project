#pragma once

#include <string_view>

// Build/link sentinel. Defined out-of-line in sentinel.cc so that any consumer
// calling these proves it actually links against the itch library.
namespace itch {

[[nodiscard]] int sentinel_value() noexcept;
[[nodiscard]] std::string_view library_name() noexcept;

} // namespace itch
