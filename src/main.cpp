// COPYRIGHT (C) sodamouse - See LICENSE.md

// TODO (Mads): Adding new entries
// TODO (Mads): Deleting entries
// TODO (Mads): Save
// TODO (Mads): Save as...
// TODO (Mads): File / edit / entry menus

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"
#include "parson.h"
#include <GLFW/glfw3.h>

#include <cassert>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>

#define ARRAY_SZ(array) sizeof(array) / sizeof(const char*)

// clang-format off
enum Platform {
    // It is not possible for a game to not have a platform,
    // therefore no PL_NONE
    PC,
    PSX, PS2, PS3, PS4,
    XBOX, X360, XONE,
    NES, SNES, GB, GBC, WII, WIIU, SWITCH,
    PC98, PC99,
};

const char* platformStr[] {
    "PC",
    "PSX", "PS2", "PS3", "PS4",
    "XBOX", "X360", "XONE",
    "NES", "SNES", "GB", "GBC", "Wii", "WiiU", "Switch",
    "PC98", "PC99",
};
// clang-format on

enum Region {
    RE_NONE,
    EU,
    JP,
    UK,
    US,
};

const char* regionStr[] {"-", "EU", "JP", "UK", "US"};

enum ContentStatus {
    CS_NONE,
    AVAILABLE,
    DOWNLOADED,
    NOT_AVAILABLE,
};

const char* contentStatusStr[] {"-", "Available", "Downloaded", "Not Available"};

enum Completion {
    CO_NONE,
    BEATEN,
    COMPLETED,
    ENDLESS,
    RETIRED,
};

const char* completionStr[] {
    "-", "Beaten", "Completed", "Endless", "Retired",
};

struct Entry
{
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

    auto* j = json_parse_file(fp);
    assert(json_value_get_type(j) == JSONArray);

    auto* jsonEntries = json_value_get_array(j);
    JSON_Object* jsonEntry;

    for (size_t i = 0; i < json_array_get_count(jsonEntries); ++i)
    {
        jsonEntry = json_array_get_object(jsonEntries, i);
        auto* e = create_entry();

        e->title = json_object_dotget_string(jsonEntry, "title");
        e->sortingTitle = json_object_dotget_string(jsonEntry, "sorting title");
        e->platform = (Platform)json_object_dotget_number(jsonEntry, "platform");
        e->region = (Region)json_object_dotget_number(jsonEntry, "region");
        e->releaseYear = json_object_dotget_number(jsonEntry, "release year");
        e->updateStatus = (ContentStatus)json_object_dotget_number(jsonEntry, "update");
        e->archivedVersion = json_object_dotget_string(jsonEntry, "archived version");
        e->bestVersion = json_object_dotget_string(jsonEntry, "best version");
        e->dlcStatus = (ContentStatus)json_object_dotget_number(jsonEntry, "content");
        e->completion = (Completion)json_object_dotget_number(jsonEntry, "completion");
        e->rating = json_object_dotget_number(jsonEntry, "rating");
        e->s = json_object_dotget_boolean(jsonEntry, "s");
        e->j = json_object_dotget_boolean(jsonEntry, "j");
        e->t = json_object_dotget_boolean(jsonEntry, "t");
        e->lastPlayed = json_object_dotget_number(jsonEntry, "last played");
    }
}

#define reset_database() entryIdx = 0

void save_database(const char* fp) { assert(false && "TODO!"); }

void draw_table()
{
    // TODO (Mads): Create a table with entry details.
    // [DONE] Entries should be in rows
    // [DONE] Entry fields should be in columns
    // Columns should be sortable alphabetically
    // The entire table should be searchable
    // [DONE] All cells should be editable
    // [DONE] Table headers
    // Make columns resizable
    // Improve row elemen sizes

    static auto flags = ImGuiTableFlags_Resizable;
    if (ImGui::BeginTable("Entries", 15, flags))
    {
        ImGui::TableSetupColumn("Title");
        ImGui::TableSetupColumn("Sorting Title");
        ImGui::TableSetupColumn("Platform");
        ImGui::TableSetupColumn("Region");
        ImGui::TableSetupColumn("Release Year");
        ImGui::TableSetupColumn("Update Status");
        ImGui::TableSetupColumn("Archived Version");
        ImGui::TableSetupColumn("Best Version");
        ImGui::TableSetupColumn("DLC");
        ImGui::TableSetupColumn("Completion");
        ImGui::TableSetupColumn("Ratin");
        ImGui::TableSetupColumn("S");
        ImGui::TableSetupColumn("J");
        ImGui::TableSetupColumn("T");
        ImGui::TableSetupColumn("Last Played");
        ImGui::TableHeadersRow();

        for (size_t i = 0; i < entryIdx; ++i)
        {
            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].title);
            ImGui::InputText("##On", &ENTRIES[i].title);
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].sortingTitle);
            ImGui::InputText("##On", &ENTRIES[i].sortingTitle);
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].platform);
            if (ImGui::BeginCombo("##ON", platformStr[ENTRIES[i].platform]))
            {
                for (int n = 0; n < ARRAY_SZ(platformStr); ++n)
                {
                    const bool isSelected = (ENTRIES[i].platform == n);
                    if (ImGui::Selectable(platformStr[n], isSelected))
                        ENTRIES[i].platform = (Platform)n;

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].region);
            if (ImGui::BeginCombo("##ON", regionStr[ENTRIES[i].region]))
            {
                for (int n = 0; n < ARRAY_SZ(regionStr); ++n)
                {
                    const bool isSelected = (ENTRIES[i].region == n);
                    if (ImGui::Selectable(regionStr[n], isSelected))
                        ENTRIES[i].region = (Region)n;

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].releaseYear);
            ImGui::InputInt("##On", &ENTRIES[i].releaseYear);
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].updateStatus);
            if (ImGui::BeginCombo("##On", contentStatusStr[ENTRIES[i].updateStatus]))
            {
                for (int n = 0; n < ARRAY_SZ(contentStatusStr); ++n)
                {
                    const bool isSelected = (ENTRIES[i].updateStatus == n);
                    if (ImGui::Selectable(contentStatusStr[n], isSelected))
                        ENTRIES[i].updateStatus = (ContentStatus)n;

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].archivedVersion);
            ImGui::InputText("##On", &ENTRIES[i].archivedVersion);
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].bestVersion);
            ImGui::InputText("##On", &ENTRIES[i].bestVersion);
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].dlcStatus);
            if (ImGui::BeginCombo("##On", contentStatusStr[ENTRIES[i].dlcStatus]))
            {
                for (int n = 0; n < 4; ++n)
                {
                    const bool isSelected = (ENTRIES[i].dlcStatus == n);
                    if (ImGui::Selectable(contentStatusStr[n], isSelected))
                        ENTRIES[i].dlcStatus = (ContentStatus)n;

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].completion);
            if (ImGui::BeginCombo("##ON", completionStr[ENTRIES[i].completion]))
            {
                for (int n = 0; n < ARRAY_SZ(completionStr); ++n)
                {
                    const bool isSelected = (ENTRIES[i].completion == n);
                    if (ImGui::Selectable(completionStr[n], isSelected))
                        ENTRIES[i].completion = (Completion)n;

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].rating);
            ImGui::InputInt("##On", &ENTRIES[i].rating);
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].s);
            ImGui::Checkbox("S", &ENTRIES[i].s);
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].j);
            ImGui::Checkbox("J", &ENTRIES[i].j);
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].t);
            ImGui::Checkbox("T", &ENTRIES[i].t);
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].lastPlayed);
            ImGui::InputInt("##On", &ENTRIES[i].lastPlayed);
            ImGui::PopID();
        }

        ImGui::EndTable();
    }
}

int main()
{
    // TODO (Mads): Config file and argument handling

    assert(glfwInit());
    GLFWwindow* window = glfwCreateWindow(800, 600, "Amelie", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("jetbrains.ttf", 18);
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    load_database("test.json");
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0, 0.2, 0.4, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        static auto flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::Begin("Amelie", nullptr, flags);
        {
            draw_main_menu();
            draw_table();
            ImGui::ShowDemoWindow();
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}
