#pragma once

#include "integer.hpp"
#include <string>
#include <vector>

namespace Excella {

constexpr const char* version = "Excella v1.12.0";
extern std::string activeDbPath;
extern std::string cacheFilePath;
extern std::vector<std::string> cachedDbPaths;
extern size_t startHash;
extern int actualTotalEntries;
extern bool dirty;

} // namespace Excella
