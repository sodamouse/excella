#include "entry.hpp"
#include "excella.hpp"

Entry entries[MAX_ENTRIES];
u64 entryIdx = 0;

Entry* create_entry()
{
    Excella::dirty = true;
    ++Excella::actualTotalEntries;

    return &entries[entryIdx++];
}

std::map<std::string, std::vector<std::string>> urls;