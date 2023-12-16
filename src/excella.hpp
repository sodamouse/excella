#pragma once

#include "integer.hpp"
#include <string>
#include <vector>

struct GLFWwindow;

namespace Excella {

constexpr const char* version = "Excella v1.14.0";
extern std::string activeDbPath;
extern std::string cacheFilePath;
extern std::vector<std::string> cachedDbPaths;
extern u64 startHash;
extern int actualTotalEntries;
extern bool dirty;
extern GLFWwindow* window;

} // namespace Excella
