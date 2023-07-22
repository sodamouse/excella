#include "database.hpp"
#include "amelie.hpp"
#include "entry.hpp"

#include <nlohmann/json.hpp>

#include <cassert>
#include <filesystem>
#include <fstream>

bool create_database(const char* fp)
{
    std::filesystem::create_directories(std::filesystem::path(fp).parent_path());
    std::fstream file(fp, std::ios::out);
    if (file)
    {
        return true;
    }

    else
    {
        return false;
    }

    return true;
}

bool load_database(const char* fp)
{
    // The fact that json::parse() needs to finish in order for this function to proceed
    // is a bottleneck in front of multi-threading. Misagh - 2023-07-18

    if (!std::filesystem::exists(fp))
    {
        printf("Database not found. Creating...");
        return create_database(fp);
    }

    std::fstream file(fp, std::ios::in);

    using namespace nlohmann;
    auto j = json::parse(file);

    for (const auto& i : j)
    {
        auto* e = create_entry();

        e->title = i["title"];
        e->sortingTitle = i["sorting title"];
        e->platform = i["platform"];
        e->region = i["region"];
        e->releaseYear = i["release year"];
        e->updateStatus = i["update"];
        e->archivedVersion = i["archived version"];
        e->bestVersion = i["best version"];
        e->dlcStatus = i["content"];
        e->completion = i["completion"];
        e->rating = i["rating"];
        e->s = i["s"];
        e->j = i["j"];
        e->t = i["t"];
        e->lastPlayed = i["last played"];
        e->notes = i["notes"];
    }

    Amelie::activeDbPath = fp;

    return true;
}

void save_database_to_file(const char* fp)
{
    using namespace nlohmann;
    json j = json::array();

    for (size_t i = 0; i < entryIdx; ++i)
    {
        if (ENTRIES[i].deleted)
            continue;

        json jObject;
        jObject["title"] = ENTRIES[i].title.c_str();
        jObject["sorting title"] = ENTRIES[i].sortingTitle.c_str();
        jObject["platform"] = ENTRIES[i].platform;
        jObject["region"] = ENTRIES[i].region;
        jObject["release year"] = ENTRIES[i].releaseYear;
        jObject["update"] = ENTRIES[i].updateStatus;
        jObject["archived version"] = ENTRIES[i].archivedVersion.c_str();
        jObject["best version"] = ENTRIES[i].bestVersion.c_str();
        jObject["content"] = ENTRIES[i].dlcStatus;
        jObject["completion"] = ENTRIES[i].completion;
        jObject["rating"] = ENTRIES[i].rating;
        jObject["s"] = ENTRIES[i].s;
        jObject["j"] = ENTRIES[i].j;
        jObject["t"] = ENTRIES[i].t;
        jObject["last played"] = ENTRIES[i].lastPlayed;
        jObject["notes"] = ENTRIES[i].notes;

        j.push_back(jObject);
    }

    std::fstream file(fp, std::ios::out);
    file << j;
}

void reset_database()
{
    for (size_t i = 0; i < entryIdx; ++i)
    {
        ENTRIES[i] = {};
    }
    entryIdx = 0;
    Amelie::actualTotalEntries = 0;
}
