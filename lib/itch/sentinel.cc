#include "itch/sentinel.hh"

namespace itch {

int sentinel_value() noexcept { return 42; }

std::string_view library_name() noexcept { return "nasdaq_itch"; }

} // namespace itch
