#pragma once

#include "integer.hpp"
#include <string>
#include <vector>

namespace Excella {

constexpr const char* version = "Excella v1.10.1";
extern std::string activeDbPath;
extern std::string cachedDbPathsFilePath;
extern std::vector<std::string> cachedDbPaths;
extern size_t startHash;
extern int actualTotalEntries;

} // namespace Excella
