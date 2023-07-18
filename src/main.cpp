// COPYRIGHT (C) sodamouse - See LICENSE.md

#include "comfyg.hpp"
#include "miriam.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_stdlib.h"

#include "stb_image.h"
#include <GLFW/glfw3.h>
#include <nlohmann/json.hpp>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <filesystem>
#include <fstream>

#define ARRAY_SZ(array) sizeof(array) / sizeof(const char*)
#define TODO assert(false && "Not implemented")

using size_t = std::size_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;

namespace Amelie {
const char* version = "v1.2.0";
const char* activeDbPath;
} // namespace Amelie

// clang-format off
enum Platform : u32 {
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

enum Region : u32 {
    RE_NONE,
    EU,
    JP,
    UK,
    US,
};

const char* regionStr[] {"-", "EU", "JP", "UK", "US"};

enum ContentStatus : u32 {
    CS_NONE,
    AVAILABLE,
    DOWNLOADED,
    NOT_AVAILABLE,
};

const char* contentStatusStr[] {"-", "Available", "Downloaded", "Not Available"};

enum Completion : u32 {
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
    i32 releaseYear = -1;

    ContentStatus updateStatus = CS_NONE;
    std::string archivedVersion = "Auto-Steam";
    std::string bestVersion = "Auto-Steam";
    ContentStatus dlcStatus = CS_NONE;
    Completion completion = CO_NONE;

    // A -1 value means not-rated
    i32 rating = -1;

    // Boolean values are assumed false until proven otherwise.
    bool s = false;
    bool j = false;
    bool t = false;

    // A -1 value means never played
    i32 lastPlayed = -1;
};

constexpr u32 ENTRIES_MAX = 1000;
Entry ENTRIES[ENTRIES_MAX];
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
        Miriam::log_info("Created database file.");
        return true;
    }

    else
    {
        fprintf(stderr, "Could not create database file.");
        return false;
    }

    return true;
}

bool load_database(const char* fp)
{
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
    }

    printf("[INFO] Database loaded: %lu entries\n", entryIdx);
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

        j.push_back(jObject);
    }

    std::fstream file(fp, std::ios::out);
    file << j;
}

#define reset_database() entryIdx = 0
// FIXME (Mads): It's confusing when new entries and their data are not fully deleted

struct Texture
{
    GLuint data;
    i32 width;
    i32 height;
};

Texture load_texture_from_file(const char* filename)
{
    Texture texture;

    // Read bytes from file
    unsigned char* imageData = stbi_load(filename, &texture.width, &texture.height, NULL, 4);
    if (imageData == NULL)
    {
        fprintf(stderr, "Could not load texture");
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

void draw_db_path_dialog() { TODO; }

void draw_main_menu(GLFWwindow* window)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ImGui::Separator();

            if (ImGui::MenuItem("New database..."))
            {
                // TODO (Mads): Open a file dialogue to specify new db's path
                // reset_database();
                // create_database("./foo.db");
            }

            if (ImGui::MenuItem("Open database...", "CTRL+o"))
            {
                TODO;
            }

            if (ImGui::MenuItem("Save", "CTRL+s"))
            {
                save_database_to_file(Amelie::activeDbPath);
            }

            if (ImGui::MenuItem("Save as...", "CTRL+SHIFT+s"))
            {
                TODO;
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Quit", "CTRL+q"))
            {
                glfwSetWindowShouldClose(window, true);
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

void draw_table()
{
    // TODO (Mads): Create a table with entry details.
    // The entire table should be searchable

    static auto flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY;
    ImGuiContext& g = *ImGui::GetCurrentContext();

    static ImVec2 cellPadding(1.0f, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cellPadding);
    if (ImGui::BeginTable("Entries", 16, flags))
    {
        ImGuiTable* table = g.CurrentTable;

        static auto flags = ImGuiTableColumnFlags_WidthFixed;
        ImGui::TableSetupColumn("Title", flags, 500.0);
        ImGui::TableSetupColumn("Sorting Title");
        ImGui::TableSetupColumn("Platform");
        ImGui::TableSetupColumn("Region", flags, 50.0);
        ImGui::TableSetupColumn("Release Year");
        ImGui::TableSetupColumn("Update Status");
        ImGui::TableSetupColumn("Archived Version");
        ImGui::TableSetupColumn("Best Version");
        ImGui::TableSetupColumn("DLC");
        ImGui::TableSetupColumn("Completion");
        ImGui::TableSetupColumn("Rating");
        ImGui::TableSetupColumn("S", flags, 25.0);
        ImGui::TableSetupColumn("J", flags, 25.0);
        ImGui::TableSetupColumn("T", flags, 25.0);
        ImGui::TableSetupColumn("Last Played");
        ImGui::TableSetupColumn("X", flags | ImGuiTableColumnFlags_NoSort, 25.0);

        ImGui::TableSetupScrollFreeze(0, 1); // TODO (Mads): What the fuck is this?
        ImGui::TableHeadersRow();

        for (size_t i = 0; i < entryIdx; ++i)
        {
            if (ENTRIES[i].deleted)
                continue;

            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::PushID(&ENTRIES[i].title);
            ImGui::PushItemWidth(-1);
            ImGui::InputTextWithHint("##On", "title", &ENTRIES[i].title);
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].sortingTitle);
            ImGui::PushItemWidth(-1);
            ImGui::InputTextWithHint("##On", "sorting title", &ENTRIES[i].sortingTitle);
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
            if (ImGui::BeginCombo("##On", regionStr[ENTRIES[i].region]))
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
            ImGui::InputTextWithHint("##On", "archived version", &ENTRIES[i].archivedVersion);
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].bestVersion);
            ImGui::PushItemWidth(-1);
            ImGui::InputTextWithHint("##On", "known best version", &ENTRIES[i].bestVersion);
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].dlcStatus);
            ImGui::PushItemWidth(-1);
            if (ImGui::BeginCombo("##On", contentStatusStr[ENTRIES[i].dlcStatus]))
            {
                for (size_t n = 0; n < 4; ++n)
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
            if (ImGui::BeginCombo("##On", completionStr[ENTRIES[i].completion]))
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
            // TODO (Mads): Embed texture into binary
            static Texture trashcan = load_texture_from_file("trashcan.jpg");
            if (ImGui::ImageButton("", (void*)(intptr_t)trashcan.data, ImVec2(18, 18)))
            {
                ENTRIES[i].deleted = true;
            }
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            // Highlight entries based on archive status
            static auto orange = ImVec4(0.6f, 0.4f, 0.2f, 1.0f);
            static auto red = ImVec4(0.6f, 0.2f, 0.2f, 1.0f);
            static auto yellow = ImVec4(0.6f, 0.6f, 0.0f, 1.0f);

            auto& e = ENTRIES[i];
            switch (e.updateStatus)
            {
            case AVAILABLE:
                table->RowBgColor[1] = ImGui::GetColorU32(yellow);
                break;
            case NOT_AVAILABLE:
                table->RowBgColor[1] = ImGui::GetColorU32(red);
                break;
            default:
                break;
            }

            switch (e.dlcStatus)
            {
            case AVAILABLE:
                table->RowBgColor[1] = ImGui::GetColorU32(orange);
                break;
            case NOT_AVAILABLE:
                table->RowBgColor[1] = ImGui::GetColorU32(red);
                break;
            default:
                break;
            }

            // Highlighting hovered row
            ImGui::TableSetColumnIndex(table->Columns.size() -
                                       1); // Jump to last column in case not enough has been drawn

            static ImRect rowRect(table->WorkRect.Min.x, table->RowPosY1, table->WorkRect.Max.x,
                                  table->RowPosY2);

            rowRect.Min.x = table->WorkRect.Min.x;
            rowRect.Min.y = table->RowPosY1;
            rowRect.Max.x = table->WorkRect.Max.x;
            rowRect.Max.y = table->RowPosY2;

            rowRect.ClipWith(table->BgClipRect);

            bool hover = ImGui::IsMouseHoveringRect(rowRect.Min, rowRect.Max, false) &&
                         ImGui::IsWindowHovered(ImGuiHoveredFlags_None);
            // !ImGui::IsAnyItemHovered();

            if (hover)
                table->RowBgColor[1] =
                    ImGui::GetColorU32(ImGuiCol_Border); // set to any color of your choice
        }

        // Sorting table entries
        if (auto* specs = ImGui::TableGetSortSpecs())
        {
            if (specs->SpecsDirty)
            {
                // TODO (Mads):
                // - sorting for the other columns
                // - reverse sorting
                std::sort(&ENTRIES[0], &ENTRIES[entryIdx],
                          [](const Entry& lhs, const Entry& rhs) -> bool {
                              auto numerical = lhs.sortingTitle.compare(rhs.sortingTitle);
                              if (numerical < 0)
                                  return true;

                              return false;
                          });
            }

            specs->SpecsDirty = false;
        }
        ImGui::EndTable();
    }
    ImGui::PopStyleVar();
}

int main()
{
    std::string username = std::getenv("USER");
    std::string configFilePath = "/home/" + username + "/.config/amelie/amelie.conf";
    const char** dbPath = Comfyg::config_str("database_path", "amelie.db");
    Comfyg::load_config_file(configFilePath.c_str());

    if (!load_database(*dbPath))
    {
        fprintf(stderr, "Could not load or create database. Aborting");
        return 1;
    }

    if (!glfwInit())
    {
        fprintf(stderr, "Could not initialize GLFW. Aborting.");
        return 1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "Amelie", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    // TODO (Mads): Embed font into binary
    io.Fonts->AddFontFromFileTTF("jetbrains.ttf", 18);
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    while (!glfwWindowShouldClose(window))
    {
        // if (ImGui::IsKeyDown(ImGuiMod_Ctrl) && ImGui::IsKeyPressed(ImGuiKey_Q))
        // {
        //     glfwSetWindowShouldClose(window, true);
        // }

        // if (ImGui::IsKeyDown(ImGuiMod_Ctrl) && ImGui::IsKeyPressed(ImGuiKey_S))
        // {
        //     save_database_to_file(Amelie::activeDbPath);
        // }

        // if (ImGui::IsKeyDown(ImGuiMod_Ctrl) && ImGui::IsKeyPressed(ImGuiKey_N))
        // {
        //     create_entry();
        // }

        if (ImGui::IsKeyDown(ImGuiKey_CapsLock) && ImGui::IsKeyPressed(ImGuiKey_Q))
        {
            glfwSetWindowShouldClose(window, true);
        }

        if (ImGui::IsKeyDown(ImGuiKey_CapsLock) && ImGui::IsKeyPressed(ImGuiKey_S))
        {
            save_database_to_file(Amelie::activeDbPath);
        }

        if (ImGui::IsKeyDown(ImGuiKey_CapsLock) && ImGui::IsKeyPressed(ImGuiKey_N))
        {
            create_entry();
        }

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
            draw_main_menu(window);
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
