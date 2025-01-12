#include "excella.hpp"
#include "core/integer.hpp"

namespace Excella {

std::string activeDbPath;
std::string cacheFilePath;
std::vector<std::string> cachedDbPaths;
int actualTotalEntries = 0;
bool dirty = false;
bool entryLoaderThreadFinished = true;
GLFWwindow* window = nullptr;
std::atomic<bool> quit = false;

} // namespace Excella
