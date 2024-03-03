#pragma once

#include "core/integer.hpp"
#include <string>
#include <vector>
#include <atomic>

struct GLFWwindow;

namespace Excella {

constexpr const char* version = "Excella v1.16.0";
extern std::string activeDbPath;
extern std::string cacheFilePath;
extern std::vector<std::string> cachedDbPaths;
extern int actualTotalEntries;
extern bool dirty;
extern bool entryLoaderThreadFinished;
extern GLFWwindow* window;
extern std::atomic<bool> quit;

} // namespace Excella
