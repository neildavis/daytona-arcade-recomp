#pragma once

#include "runtime/rom_import.h" // Defines M2_ROMSET, defaulting to Deluxe '93.

#include <string_view>

namespace rt {

// The ROM revision this executable was built for; evaluated at compile time.
inline constexpr bool kIsRevisionA =
    std::string_view{M2_ROMSET} == "daytona";

} // namespace rt
