// COPYRIGHT (C) sodamouse - See LICENSE.md

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "json.hpp"
#include <GLFW/glfw3.h>

#include <cassert>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>

enum Platform {
    PC,
};

enum Region {
    RE_NONE,
    EU,
    JP,
    UK,
    US,
};

enum ContentStatus {
    CS_NONE,
    AVAILABLE,
    DOWNLOADED,
    NOT_AVAILABLE,
};

enum Completion {
    CO_NONE,
    BEATEN,
    COMPLETED,
    ENDLESS,
    RETIRED,
};

struct Entry
{
    // Our json library is forcing us to use std::string.
    // This should be rectified with a more C-like library.
    // We want const char*s.
    // Mads, July 06 2023
    std::string title;
    std::string sortingTitle;

    Platform platform;
    Region region;
    int releaseYear;
    ContentStatus updateStatus;
    std::string archivedVersion;
    std::string bestVersion;
    ContentStatus dlcStatus;
    Completion completion;

    // A -1 value means not-rated
    int rating;

    // Boolean values are assumed false until proven otherwise.
    bool s;
    bool j;
    bool t;

    // A -1 value means never played
    int lastPlayed;
};

void print_entry(const Entry* entry)
{
    // TODO (Mads): Should eventually just return a string containing full entry details
    // Might accept a stream (eg. FILE*, etc.)
    std::cout << std::boolalpha;
    std::cout << entry->title << '\n';
    std::cout << '\t' << entry->sortingTitle << '\n';

    switch (entry->platform) {
    case PC:
        std::cout << "\tPC" << '\n'; break;
    };

    switch (entry->region) {
    case RE_NONE: std::cout << "\t-" << '\n'; break;
    case EU: std::cout << "\tEU" << '\n'; break;
    case JP: std::cout << "\tJP" << '\n'; break;
    case UK: std::cout << "\tUK" << '\n'; break;
    case US: std::cout << "\tUS" << '\n'; break;
    }

    std::cout << '\t' << entry->releaseYear << '\n';

    switch (entry->updateStatus) {
    case CS_NONE:       std::cout << "\t-" << '\n'; break;
    case AVAILABLE:     std::cout << "\tAvailable" << '\n'; break;
    case DOWNLOADED:    std::cout << "\tDownloaded" << '\n'; break;
    case NOT_AVAILABLE: std::cout << "\tNot Available" << '\n'; break;
    }

    std::cout << '\t' << entry->archivedVersion << '\n';
    std::cout << '\t' << entry->bestVersion << '\n';

    switch (entry->dlcStatus) {
    case CS_NONE:       std::cout << "\t-" << '\n'; break;
    case AVAILABLE:     std::cout << "\tAvailable" << '\n'; break;
    case DOWNLOADED:    std::cout << "\tDownloaded" << '\n'; break;
    case NOT_AVAILABLE: std::cout << "\tNot Available" << '\n'; break;
    }

    switch (entry->completion) {
    case CO_NONE:   std::cout << "\t-" << '\n'; break;
    case BEATEN:    std::cout << "\tBeaten" << '\n'; break;
    case COMPLETED: std::cout << "\tCompleted" << '\n'; break;
    case ENDLESS:   std::cout << "\tEndless" << '\n'; break;
    case RETIRED:   std::cout << "\tRetired" << '\n'; break;
    }

    std::cout << '\t' << entry->rating << '\n';
    std::cout << '\t' << entry->s << '\n';
    std::cout << '\t' << entry->j << '\n';
    std::cout << '\t' << entry->t << '\n';
    std::cout << '\t' << entry->lastPlayed << '\n';
}


#define ENTRIES_MAX 1000
Entry ENTRIES[ENTRIES_MAX];
using size_t = std::size_t;
size_t entryIdx = 0;

Entry* create_entry()
{
    Entry* e = &ENTRIES[entryIdx++];
    return e;
}

void load_database(const char* fp)
{
    assert(std::filesystem::exists(fp));

    using json = nlohmann::json;
    std::fstream file(fp, std::ios::in);
    json j;
    file >> j;

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
    }
}

#define reset_database() entryIdx = 0

void save_database(const char* fp)
{
    // TODO (Mads)
}

int main()
{
    // TODO (Mads): Config file and argument handling

    assert(glfwInit());
    GLFWwindow* window = glfwCreateWindow(800, 600, "Amelie", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0, 0.2, 0.4, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        static bool databaseLoaded = false;
        ImGui::Begin("Amelie");
        {
            if (ImGui::Button("Load db"))
            {
                reset_database();
                load_database("test.json");
                databaseLoaded = true;
            }

            if (ImGui::Button("Close db"))
            {
                reset_database();
                databaseLoaded = false;
            }

            if (databaseLoaded)
            {
                // TODO (Mads): Create a table with entry details.
                // Entry should be in sortable columns.
                // The table should be searchable
                for (size_t i = 0; i < entryIdx; ++i)
                {
                    ImGui::Text("%s", ENTRIES[i].title.c_str());
                }
            }
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}
