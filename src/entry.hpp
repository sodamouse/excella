#pragma once

#include "integer.hpp"
#include <string>

// clang-format off
enum Platform : u32 {
    PC,
    PSX, PS2, PS3, PS4,
    XBOX, X360, XONE,
    NES, SNES, GB, GBA, GBC, GC, DS, DS3, WII, WIIU, SWITCH,
    PC98, PC99,
    SG_1000, MASTER_SYSTEM, GENESIS, GAME_GEAR, SEGA_CD, SEGA_PICO, X32, SATURN, DREAMCAST, ADVANCED_PICO
};

constexpr const char* platformStr[] {
    "PC",
    "PSX", "PS2", "PSP", "PS3", "PS4",
    "XBOX", "X360", "XONE",
    "NES", "SNES", "GB", "GBA", "GBC", "GC", "DS", "3DS", "Wii", "WiiU", "Switch",
    "PC98", "PC99",
    "SG-100", "Master System", "Genesis", "Game Gear", "Sega CD", "Sega Pico", "32X", "Saturn", "Dreamcast", "APB"
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
    LOCKED,
};

constexpr const char* contentStatusStr[] {"-", "Available", "Downloaded", "Not Available",
                                          "Locked"};

enum Completion : u32 {
    CO_NONE,
    BEATEN,
    COMPLETED,
    ENDLESS,
    RETIRED,
    UNPLAYABLE,
    WAITING,
    WATCHED,
    SKIPPED,
};

constexpr const char* completionStr[] {
    "-", "Beaten", "Completed", "Endless", "Retired", "Unplayable", "Waiting", "Watched", "Skipped"
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

    std::string notes;
};

constexpr size_t ENTRIES_MAX = 2000;    // double every time the capacity is exceeded
extern Entry ENTRIES[];
extern size_t entryIdx;
Entry* create_entry();
