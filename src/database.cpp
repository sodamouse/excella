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
    for (u64 i = 0; i < entryIdx; ++i) entries[i] = {};
    entryIdx = 0;
    Excella::actualTotalEntries = 0;
}

void load_database(const char* fp)
{
    Excella::entryLoaderThreadFinished = false;

    if (!std::filesystem::exists(fp))
    {
        std::cout << "Database not found. Creating...\n";
        create_database(fp);
    }

    std::fstream in(fp, std::ios::in | std::ios::binary);

    // @TODO (Mads): Query EXCL header. If file type is wrong, do not crash! (2023-08-16)
    u64 count = 0;
    in.read((char*)&count, sizeof(u64));
    for (u32 i = 0; i < count; ++i)
    {
        Entry* e = create_entry();

        // title
        u64 size = 0;
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

        // tags
        size = 0;
        in.read((char*)&size, sizeof(size));
        for (u64 i = 0; i < size; ++i)
        {
            u64 tagSize = 0;
            std::string tag;
            in.read((char*)&tagSize, sizeof(tagSize));
            tag.resize(tagSize);
            in.read((char*)tag.data(), tagSize);
            e->tags.push_back(tag);
        }
    }

    // URLs
    in.read((char*)&count, sizeof(count));
    for (u64 i = 0; i < count; ++i)
    {
        u64 size = 0;
        in.read((char*)&size, sizeof(size));
        std::string url;
        url.resize(size);
        in.read((char*)url.c_str(), size);

        std::vector<std::string> titles;
        u64 titleCount = 0;
        in.read((char*)&titleCount, sizeof(titleCount));
        for (u64 j = 0; j < titleCount; ++j)
        {
            in.read((char*)&size, sizeof(size));
            std::string title;
            title.resize(size);
            in.read((char*)title.c_str(), size);
            titles.push_back(title);
        }

        urls[url] = titles;
    }

    Excella::activeDbPath = fp;

    if (std::find(Excella::cachedDbPaths.begin(), Excella::cachedDbPaths.end(),
                  Excella::activeDbPath) == Excella::cachedDbPaths.end())
    {
        Excella::cachedDbPaths.push_back(fp);
        save_cache_file();
    }

    Excella::dirty = false;
    Excella::entryLoaderThreadFinished = true;
}

void save_database(const char* fp)
{
    if (!Excella::dirty) return;
    if (!Excella::entryLoaderThreadFinished) return;
    Excella::entryLoaderThreadFinished = false;

    std::fstream out(fp, std::ios::out | std::ios::binary);
    assert(out.is_open() && "Could not create output file");    // @HACK runtime error checking

    // @TODO (Mads): Write the magic number into the header (2023-08-16)
    out.write((const char*)&entryIdx, sizeof(entryIdx));
    u64 reduce = 0;

    for (u32 i = 0; i < entryIdx; ++i)
    {
        if (entries[i].deleted)
        {
            ++reduce;
            continue;
        }

        // title
        auto size = entries[i].title.size();
        out.write((const char*)&size, sizeof(size));
        out.write(entries[i].title.data(), size);

        // sortingTitle
        size = entries[i].sortingTitle.size();
        out.write((const char*)&size, sizeof(size));
        out.write(entries[i].sortingTitle.data(), size);

        // platform
        out.write((const char*)&entries[i].platform, sizeof(u32));

        // region
        out.write((const char*)&entries[i].region, sizeof(u32));

        // releaseYear
        out.write((const char*)&entries[i].releaseYear, sizeof(i32));

        // updateStatus
        out.write((const char*)&entries[i].updateStatus, sizeof(u32));

        // archivedVersion
        size = entries[i].archivedVersion.size();
        out.write((const char*)&size, sizeof(size));
        out.write(entries[i].archivedVersion.data(), size);

        // bestVersion
        size = entries[i].bestVersion.size();
        out.write((const char*)&size, sizeof(size));
        out.write(entries[i].bestVersion.data(), size);

        // dlcStatus
        out.write((const char*)&entries[i].dlcStatus, sizeof(u32));

        // completion
        out.write((const char*)&entries[i].completion, sizeof(u32));

        // rating
        out.write((const char*)&entries[i].rating, sizeof(i32));

        // s
        out.write((const char*)&entries[i].s, sizeof(bool));

        // j
        out.write((const char*)&entries[i].j, sizeof(bool));

        // t
        out.write((const char*)&entries[i].t, sizeof(bool));

        // lastPlayed
        out.write((const char*)&entries[i].lastPlayed, sizeof(i32));

        // notes
        size = entries[i].notes.size();
        out.write((const char*)&size, sizeof(size));
        out.write(entries[i].notes.data(), size);

        // tags
        size = entries[i].tags.size();
        out.write((const char*)&size, sizeof(size));
        for (const auto& tag : entries[i].tags)
        {
            size = tag.size();
            out.write((const char*)&size, sizeof(size));
            out.write(tag.data(), size);
        }
    }

    // URLs
    u64 count = urls.size();
    out.write((const char*)&count, sizeof(count));

    for (const auto& kv : urls)
    {
        u64 size = kv.first.length();
        out.write((const char*)&size, sizeof(size));
        out.write(kv.first.c_str(), size);

        count = kv.second.size();
        out.write((const char*)&count, sizeof(count));
        for (const auto& title : kv.second)
        {
            size = title.length();
            out.write((const char*)&size, sizeof(size));
            out.write(title.c_str(), size);
        }
    }

    // TODO (Mads): This can be simplified with a header... (2023-08-16)
    out.seekp(0);
    entryIdx -= reduce;
    out.write((const char*)&entryIdx, sizeof(entryIdx));

    Excella::dirty = false;
    Excella::entryLoaderThreadFinished = true;
}

void save_cache_file()
{
    std::fstream file(Excella::cacheFilePath, std::ios::out);
    if (!file.is_open()) return;

    for (const auto& line : Excella::cachedDbPaths)
    {
        file << line;
        file << '\n';
    }
}