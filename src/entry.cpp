#include "entry.hpp"
#include "amelie.hpp"

Entry ENTRIES[ENTRIES_MAX];
size_t entryIdx = 0;

Entry* create_entry()
{
    ++Amelie::actualTotalEntries;
    return &ENTRIES[entryIdx++];
}
