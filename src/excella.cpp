#include "excella.hpp"
#include "core/integer.hpp"

namespace Excella {

std::string activeDbPath;
std::string cacheFilePath;
std::vector<std::string> cachedDbPaths;
u64 startHash;
int actualTotalEntries = 0;
bool dirty = false;
bool entryLoaderThreadFinished = false;
GLFWwindow* window = nullptr;

} // namespace Excella
