#include "entry.hpp"

Entry ENTRIES[ENTRIES_MAX];
size_t entryIdx = 0;

Entry* create_entry() { return &ENTRIES[entryIdx++]; }
