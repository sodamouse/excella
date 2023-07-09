// COPYRIGHT (C) sodamouse - See LICENSE.md

#include "comfyg.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_stdlib.h"
#include "json.hpp"
#include "miriam.hpp"
#include "stb_image.h"
#include <GLFW/glfw3.h>

#include <cassert>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <format>
#include <fstream>

#define ARRAY_SZ(array) sizeof(array) / sizeof(const char*)
#define TODO assert(false && "Not implemented")

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
    bool deleted = false;

    std::string title;
    std::string sortingTitle;

    Platform platform = PC;
    Region region = RE_NONE;

    // A -1 value means release date is unknown
    int releaseYear = -1;

    ContentStatus updateStatus = CS_NONE;
    std::string archivedVersion;
    std::string bestVersion;
    ContentStatus dlcStatus = CS_NONE;
    Completion completion = CO_NONE;

    // A -1 value means not-rated
    int rating = -1;

    // Boolean values are assumed false until proven otherwise.
    bool s = false;
    bool j = false;
    bool t = false;

    // A -1 value means never played
    int lastPlayed = -1;
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

bool create_database(const char* fp)
{
    std::filesystem::create_directories(std::filesystem::path(fp).parent_path());
    std::fstream file(fp, std::ios::out);
    if (file)
    {
        log_info("Created database file.");
        return true;
    }

    else
    {
        log_error("Could not create database file.");
        return false;
    }

    return true;
}

bool load_database(const char* fp)
{
    if (!std::filesystem::exists(fp))
    {
        log_info("Database not found. Creating...");
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
    }

    log_info("Database loaded");
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
        jObject["platform"] = (int)ENTRIES[i].platform;
        jObject["region"] = (int)ENTRIES[i].region;
        jObject["release year"] = ENTRIES[i].releaseYear;
        jObject["update"] = (int)ENTRIES[i].updateStatus;
        jObject["archived version"] = ENTRIES[i].archivedVersion.c_str();
        jObject["best version"] = ENTRIES[i].bestVersion.c_str();
        jObject["content"] = (int)ENTRIES[i].dlcStatus;
        jObject["completion"] = (int)ENTRIES[i].completion;
        jObject["rating"] = ENTRIES[i].rating;
        jObject["s"] = ENTRIES[i].s;
        jObject["j"] = ENTRIES[i].j;
        jObject["t"] = ENTRIES[i].t;
        jObject["last played"] = ENTRIES[i].lastPlayed;

        j.push_back(jObject);
    }

    std::fstream file(fp, std::ios::out);
    file << j;
}

#define reset_database() entryIdx = 0

struct Texture
{
    GLuint data;
    int width;
    int height;
};

Texture load_texture_from_file(const char* filename)
{
    Texture texture;

    unsigned char* imageData = stbi_load(filename, &texture.width, &texture.height, NULL, 4);
    if (imageData == NULL)
    {
        log_error("Could not load texture");
        return texture;
    }

    // Create a OpenGL texture identifier
    glGenTextures(1, &texture.data);
    glBindTexture(GL_TEXTURE_2D, texture.data);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, imageData);
    stbi_image_free(imageData);

    return texture;
}

void draw_main_menu(const char* dbPath)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ImGui::Separator();

            if (ImGui::MenuItem("Save", "CTRL+s"))
            {
                save_database_to_file(dbPath);
            }

            if (ImGui::MenuItem("Save as...", "CTRL+SHIFT+s"))
            {
                TODO;
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Quit", "CTRL+q"))
            {
                TODO;
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Entry"))
        {
            if (ImGui::MenuItem("Create entry", "CTRL+n"))
            {
                create_entry();
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void draw_status_bar() { TODO; }

void draw_table()
{
    // TODO (Mads): Create a table with entry details.
    // Columns should be sortable alphabetically
    // The entire table should be searchable

    static auto flags = ImGuiTableFlags_Resizable;
    if (ImGui::BeginTable("Entries", 16, flags))
    {
        ImGui::TableSetupColumn("Title", ImGuiTableColumnFlags_WidthFixed, 500.0);
        ImGui::TableSetupColumn("Sorting Title");
        ImGui::TableSetupColumn("Platform");
        ImGui::TableSetupColumn("Region", ImGuiTableColumnFlags_WidthFixed, 50.0);
        ImGui::TableSetupColumn("Release Year");
        ImGui::TableSetupColumn("Update Status");
        ImGui::TableSetupColumn("Archived Version");
        ImGui::TableSetupColumn("Best Version");
        ImGui::TableSetupColumn("DLC");
        ImGui::TableSetupColumn("Completion");
        ImGui::TableSetupColumn("Rating");
        ImGui::TableSetupColumn("S", ImGuiTableColumnFlags_WidthFixed, 25.0);
        ImGui::TableSetupColumn("J", ImGuiTableColumnFlags_WidthFixed, 25.0);
        ImGui::TableSetupColumn("T", ImGuiTableColumnFlags_WidthFixed, 25.0);
        ImGui::TableSetupColumn("Last Played");
        ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_WidthFixed, 25.0);
        ImGui::TableHeadersRow();

        for (size_t i = 0; i < entryIdx; ++i)
        {
            if (ENTRIES[i].deleted)
                continue;

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].title);
            ImGui::PushItemWidth(-1);
            ImGui::InputText("##On", &ENTRIES[i].title);
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].sortingTitle);
            ImGui::PushItemWidth(-1);
            ImGui::InputText("##On", &ENTRIES[i].sortingTitle);
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].platform);
            ImGui::PushItemWidth(-1);
            if (ImGui::BeginCombo("##ON", platformStr[ENTRIES[i].platform]))
            {
                for (size_t n = 0; n < ARRAY_SZ(platformStr); ++n)
                {
                    const bool isSelected = (ENTRIES[i].platform == n);
                    if (ImGui::Selectable(platformStr[n], isSelected))
                        ENTRIES[i].platform = (Platform)n;

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].region);
            ImGui::PushItemWidth(-1);
            if (ImGui::BeginCombo("##ON", regionStr[ENTRIES[i].region]))
            {
                for (size_t n = 0; n < ARRAY_SZ(regionStr); ++n)
                {
                    const bool isSelected = (ENTRIES[i].region == n);
                    if (ImGui::Selectable(regionStr[n], isSelected))
                        ENTRIES[i].region = (Region)n;

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].releaseYear);
            ImGui::PushItemWidth(-1);
            ImGui::InputInt("##On", &ENTRIES[i].releaseYear);
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].updateStatus);
            ImGui::PushItemWidth(-1);
            if (ImGui::BeginCombo("##On", contentStatusStr[ENTRIES[i].updateStatus]))
            {
                for (size_t n = 0; n < ARRAY_SZ(contentStatusStr); ++n)
                {
                    const bool isSelected = (ENTRIES[i].updateStatus == n);
                    if (ImGui::Selectable(contentStatusStr[n], isSelected))
                        ENTRIES[i].updateStatus = (ContentStatus)n;

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].archivedVersion);
            ImGui::PushItemWidth(-1);
            ImGui::InputText("##On", &ENTRIES[i].archivedVersion);
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].bestVersion);
            ImGui::PushItemWidth(-1);
            ImGui::InputText("##On", &ENTRIES[i].bestVersion);
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].dlcStatus);
            ImGui::PushItemWidth(-1);
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
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].completion);
            ImGui::PushItemWidth(-1);
            if (ImGui::BeginCombo("##ON", completionStr[ENTRIES[i].completion]))
            {
                for (size_t n = 0; n < ARRAY_SZ(completionStr); ++n)
                {
                    const bool isSelected = (ENTRIES[i].completion == n);
                    if (ImGui::Selectable(completionStr[n], isSelected))
                        ENTRIES[i].completion = (Completion)n;

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].rating);
            ImGui::PushItemWidth(-1);
            ImGui::InputInt("##On", &ENTRIES[i].rating);
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].s);
            ImGui::PushItemWidth(-1);
            ImGui::Checkbox("##On", &ENTRIES[i].s);
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].j);
            ImGui::PushItemWidth(-1);
            ImGui::Checkbox("##On", &ENTRIES[i].j);
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].t);
            ImGui::PushItemWidth(-1);
            ImGui::Checkbox("##On", &ENTRIES[i].t);
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].lastPlayed);
            ImGui::PushItemWidth(-1);
            ImGui::InputInt("##On", &ENTRIES[i].lastPlayed);
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].deleted);
            ImGui::PushItemWidth(-1);
            static Texture trashcan = load_texture_from_file("trashcan.jpg");
            if (ImGui::ImageButton("", (void*)(intptr_t)trashcan.data, ImVec2(18, 18)))
            {
                ENTRIES[i].deleted = true;
            }
            (void)ImGui::PopItemWidth();
            ImGui::PopID();
        }

        ImGui::EndTable();
    }
}

int main()
{
    std::string username = std::getenv("USER");
    std::string configFilePath = "/home/" + username + "/.config/amelie/amelie.conf";
    const char** dbPath = Comfyg::config_str("database_path", "amelie.db");
    Comfyg::load_config_file(configFilePath.c_str());

    if (!load_database(*dbPath))
    {
        log_fatal("Could not load or create database. Aborting");
        return 1;
    }

    if (!glfwInit())
    {
        log_fatal("Could not initialize GLFW. Aborting.");
        return 1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "Amelie", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("jetbrains.ttf", 18);
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0, 0.2, 0.4, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        static auto flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                            ImGuiWindowFlags_NoSavedSettings;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::Begin("Amelie", nullptr, flags);
        {
            draw_main_menu(*dbPath);
            draw_table();
            // draw_status_bar();
            // ImGui::ShowDemoWindow();
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}
