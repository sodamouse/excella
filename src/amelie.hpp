#pragma once

#include "integer.hpp"
#include <string>

namespace Amelie {

constexpr const char* version = "Amelie v1.8.0";
extern std::string activeDbPath;
extern size_t startHash;
extern int actualTotalEntries;

} // namespace Amelie
