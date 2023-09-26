#include "database.hpp"
#include "entry.hpp"
#include "excella.hpp"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>

void create_database(const char* fp)
{
    // TODO (Mads): Double check cppreference. How does this actually behave? WTF knows.
    // true if a directory was created for the directory p resolves to, false otherwise.
    // What if directory already exists?

    std::filesystem::create_directories(std::filesystem::path(fp).parent_path());
    std::fstream file(fp, std::ios::out);

    if (!std::filesystem::exists(fp))
    {
        std::cout << "Could not create database: `" << fp << "`. Aborting.\n";
        exit(-1);
    }

    std::cout << "Created: `" << fp << "`.\n";
}

void reset_database()
{
    for (size_t i = 0; i < entryIdx; ++i)
    {
        ENTRIES[i] = {};
    }
    entryIdx = 0;
    Excella::actualTotalEntries = 0;
}

void load_database(const char* fp)
{
    if (!std::filesystem::exists(fp))
    {
        printf("Database not found. Creating...\n");
        create_database(fp);
    }

    std::fstream in(fp, in.in | in.binary);

    // TODO (Mads): Query EXCL header. If file type is wrong, do not crash! (2023-08-16)
    size_t count = 0;
    in.read((char*)&count, sizeof(size_t));
    for (u32 i = 0; i < count; ++i)
    {
        Entry* e = create_entry();

        // title
        size_t size = 0;
        in.read((char*)&size, sizeof(size));
        e->title.resize(size);
        in.read((char*)e->title.data(), size);

        // sortingTitle
        size = 0;
        in.read((char*)&size, sizeof(size));
        e->sortingTitle.resize(size);
        in.read((char*)e->sortingTitle.data(), size);

        // platform
        in.read((char*)&e->platform, sizeof(u32));

        // region
        in.read((char*)&e->region, sizeof(u32));

        // releaseYear
        in.read((char*)&e->releaseYear, sizeof(i32));

        // updateStatus
        in.read((char*)&e->updateStatus, sizeof(u32));

        // archivedVersion
        size = 0;
        in.read((char*)&size, sizeof(size));
        e->archivedVersion.resize(size);
        in.read((char*)e->archivedVersion.data(), size);

        // bestVersion
        size = 0;
        in.read((char*)&size, sizeof(size));
        e->bestVersion.resize(size);
        in.read((char*)e->bestVersion.data(), size);

        // dlcStatus
        in.read((char*)&e->dlcStatus, sizeof(u32));

        // completion
        in.read((char*)&e->completion, sizeof(u32));

        // rating
        in.read((char*)&e->rating, sizeof(i32));

        // s
        in.read((char*)&e->s, sizeof(bool));

        // j
        in.read((char*)&e->j, sizeof(bool));

        // t
        in.read((char*)&e->t, sizeof(bool));

        // lastPlayed
        in.read((char*)&e->lastPlayed, sizeof(i32));

        // notes
        size = 0;
        in.read((char*)&size, sizeof(size));
        e->notes.resize(size);
        in.read((char*)e->notes.data(), size);
    }

    Excella::activeDbPath = fp;

    if (std::find(Excella::cachedDbPaths.begin(), Excella::cachedDbPaths.end(),
                  Excella::activeDbPath) == Excella::cachedDbPaths.end())
    {
        Excella::cachedDbPaths.push_back(fp);
        std::fstream file(Excella::cacheFilePath, file.out);
        for (const auto& line : Excella::cachedDbPaths)
        {
            file << line;
            file << '\n';
        }
    }

    Excella::dirty = false;
}

void save_database(const char* fp)
{
    if (!Excella::dirty)
        return;

    std::fstream out(fp, out.out | out.binary);
    assert(out.is_open() && "Could not create output file");

    // TODO (Mads): Write the magic number into the header (2023-08-16)
    out.write((const char*)&entryIdx, sizeof(entryIdx));
    size_t reduce = 0;

    for (u32 i = 0; i < entryIdx; ++i)
    {
        if (ENTRIES[i].deleted)
        {
            ++reduce;
            continue;
        }
        // title
        auto size = ENTRIES[i].title.size();
        out.write((const char*)&size, sizeof(size));
        out.write(ENTRIES[i].title.data(), size);

        // sortingTitle
        size = ENTRIES[i].sortingTitle.size();
        out.write((const char*)&size, sizeof(size));
        out.write(ENTRIES[i].sortingTitle.data(), size);

        // platform
        out.write((const char*)&ENTRIES[i].platform, sizeof(u32));

        // region
        out.write((const char*)&ENTRIES[i].region, sizeof(u32));

        // releaseYear
        out.write((const char*)&ENTRIES[i].releaseYear, sizeof(i32));

        // updateStatus
        out.write((const char*)&ENTRIES[i].updateStatus, sizeof(u32));

        // archivedVersion
        size = ENTRIES[i].archivedVersion.size();
        out.write((const char*)&size, sizeof(size));
        out.write(ENTRIES[i].archivedVersion.data(), size);

        // bestVersion
        size = ENTRIES[i].bestVersion.size();
        out.write((const char*)&size, sizeof(size));
        out.write(ENTRIES[i].bestVersion.data(), size);

        // dlcStatus
        out.write((const char*)&ENTRIES[i].dlcStatus, sizeof(u32));

        // completion
        out.write((const char*)&ENTRIES[i].completion, sizeof(u32));

        // rating
        out.write((const char*)&ENTRIES[i].rating, sizeof(i32));

        // s
        out.write((const char*)&ENTRIES[i].s, sizeof(bool));

        // j
        out.write((const char*)&ENTRIES[i].j, sizeof(bool));

        // t
        out.write((const char*)&ENTRIES[i].t, sizeof(bool));

        // lastPlayed
        out.write((const char*)&ENTRIES[i].lastPlayed, sizeof(i32));

        // notes
        size = ENTRIES[i].notes.size();
        out.write((const char*)&size, sizeof(size));
        out.write(ENTRIES[i].notes.data(), size);
    }

    // TODO (Mads): This can be simplified with a header... (2023-08-16)
    out.seekp(0);
    entryIdx -= reduce;
    out.write((const char*)&entryIdx, sizeof(entryIdx));

    Excella::dirty = false;
}
