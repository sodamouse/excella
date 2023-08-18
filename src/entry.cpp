#include "entry.hpp"
#include "excella.hpp"

Entry ENTRIES[ENTRIES_MAX];
size_t entryIdx = 0;

Entry* create_entry()
{
    Excella::dirty = true;

    ++Excella::actualTotalEntries;
    return &ENTRIES[entryIdx++];
}
