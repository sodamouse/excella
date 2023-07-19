#pragma once

#include "integer.hpp"
#include <string>

// clang-format off
enum Platform : u32 {
    PC,
    PSX, PS2, PS3, PS4,
    XBOX, X360, XONE,
    NES, SNES, GB, GBC, WII, WIIU, SWITCH,
    PC98, PC99,
};

constexpr const char* platformStr[] {
    "PC",
    "PSX", "PS2", "PS3", "PS4",
    "XBOX", "X360", "XONE",
    "NES", "SNES", "GB", "GBC", "Wii", "WiiU", "Switch",
    "PC98", "PC99",
};
// clang-format on

enum Region : u32 {
    RE_NONE,
    EU,
    JP,
    UK,
    US,
};

constexpr const char* regionStr[] {"-", "EU", "JP", "UK", "US"};

enum ContentStatus : u32 {
    CS_NONE,
    AVAILABLE,
    DOWNLOADED,
    NOT_AVAILABLE,
};

constexpr const char* contentStatusStr[] {"-", "Available", "Downloaded", "Not Available"};

enum Completion : u32 {
    CO_NONE,
    BEATEN,
    COMPLETED,
    ENDLESS,
    RETIRED,
};

constexpr const char* completionStr[] {
    "-", "Beaten", "Completed", "Endless", "Retired",
};

struct Entry
{
    bool deleted = false;

    std::string title;
    std::string sortingTitle;

    Platform platform = PC;
    Region region = RE_NONE;

    // A -1 value means release date is unknown
    i32 releaseYear = -1;

    ContentStatus updateStatus = DOWNLOADED;
    std::string archivedVersion = "Auto-Steam";
    std::string bestVersion = "Auto-Steam";
    ContentStatus dlcStatus = CS_NONE;
    Completion completion = CO_NONE;

    // A -1 value means not-rated
    i32 rating = -1;

    bool s = false;
    bool j = false;
    bool t = false;

    // A -1 value means never played
    i32 lastPlayed = -1;
};

constexpr u32 ENTRIES_MAX = 1000;
extern Entry ENTRIES[];
extern size_t entryIdx;

Entry* create_entry();
