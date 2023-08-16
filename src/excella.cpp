#include "excella.hpp"
#include "integer.hpp"

namespace Excella {

std::string activeDbPath;
std::string cachedDbPathsFilePath;
std::vector<std::string> cachedDbPaths;
size_t startHash;
int actualTotalEntries = 0;

} // namespace Excella
