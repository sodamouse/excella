// @FEATURE (Mads): Show save prompt if excella dirty

#include "imgui.hpp"
#include "font.hpp"
#include "database.hpp"
#include "entry.hpp"
#include "excella.hpp"
#include "texture.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_stdlib.h>
#include "file_browser/filebrowser.hpp"
#include <GLFW/glfw3.h>

#ifdef OS_WINDOWS
#include <shellapi.h>
#endif

#include <algorithm>
#include <cstring>
#include <fstream>
#include <map>

void init_imgui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromMemoryTTF(fontBytes, sizeof(fontBytes), 18);
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui_ImplGlfw_InitForOpenGL(Excella::window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

// File browser
static constexpr auto BROWSER_FLAGS =
    ImGuiFileBrowserFlags_EnterNewFilename |
    ImGuiFileBrowserFlags_CloseOnEsc       |
    ImGuiFileBrowserFlags_CreateNewDir;

static ImGui::FileBrowser browser(BROWSER_FLAGS);
static bool browserWantsSave = false;
static bool browserWantsLoad = false;

// Search
static ImGuiTextFilter search;
static bool focusSearch   = false;
static bool focusNewEntry = false;

// Filter
struct Filter
{
    bool active = false;

    bool platformsSelected[COUNT_PLATFORM] {};
    bool platformActive = false;

    i32 releaseYear = 0;
    bool releaseYearActive = false;

    i32 lastPlayed = 0;
    bool lastPlayedActive = false;

    i32 rating = 0;
    bool ratingActive = false;

    bool completionsSelected[COUNT_COMPLETION] {};
    bool completionActive = false;

    bool sActive = false;
    bool jActive = false;
    bool tActive = false;
};

static Filter filter {};
bool filterNodeOpen = false;

// Tags
static std::vector<std::string> inclusiveTags;
static std::vector<std::string> exclusiveTags;

// Tag manager
static std::map<std::string, i32> currentTagsInDatabase;
static bool showTagsPopup = false;
static bool populateCurrentTagsMap = true;

// URLs
static bool showUrlsPopup = false;

// Statistics
static bool showStatisticsPopup = false;

// Pop-up
static void (*draw_popup)() = []() {};

void handle_keyboard_events();
void draw_main_menu();
void draw_file_browser();
void draw_table();

void update()
{
    focusSearch   = false;
    focusNewEntry = false;

    handle_keyboard_events();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    static constexpr auto WINDOW_FLAGS =
        ImGuiWindowFlags_NoDecoration   |
        ImGuiWindowFlags_NoMove         |
        ImGuiWindowFlags_NoSavedSettings;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    ImGui::Begin("Excella", nullptr, WINDOW_FLAGS);
    {
        draw_main_menu();
        draw_file_browser();
        draw_table();
        draw_popup();
    }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void handle_keyboard_events()
{
    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_Q)) Excella::quit = true;

    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_T))
    {
        showTagsPopup = !showTagsPopup;
        if (!showTagsPopup)
        {
            populateCurrentTagsMap = true;
            currentTagsInDatabase.clear();

            ImGui::CloseCurrentPopup();
            showTagsPopup = false;
        }
    }

    if (showTagsPopup || showUrlsPopup || showStatisticsPopup) return;

    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_S)) save_database(Excella::activeDbPath.c_str());

    auto saveDatabaseAs =
        ImGui::IsKeyDown(ImGuiKey_LeftCtrl)  &&
        ImGui::IsKeyDown(ImGuiKey_LeftShift) &&
        ImGui::IsKeyPressed(ImGuiKey_S);

    if (saveDatabaseAs)
    {
        browserWantsSave = true;
        browserWantsLoad = false;
        browser.Open();
    }

    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_N))
    {
        search.Clear();
        filter = {};
        create_entry();
        focusNewEntry = true;
    }

    if (!showTagsPopup && ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_F)) focusSearch = true;

    if (!showTagsPopup && ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_G)) filterNodeOpen = !filterNodeOpen;
}

void draw_main_menu()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ImGui::Separator();

            if (ImGui::MenuItem("Open database..."))
            {
                browser.Open();
                browserWantsSave = false;
                browserWantsLoad = true;
            }

            if (ImGui::MenuItem("Save", "CTRL+s")) save_database(Excella::activeDbPath.c_str());

            if (ImGui::MenuItem("Save as...", "CTRL+SHIFT+s"))
            {
                browser.Open();
                browserWantsSave = true;
                browserWantsLoad = false;
            }

            if (!Excella::cachedDbPaths.empty())
            {
                ImGui::Separator();

                for (const auto& line : Excella::cachedDbPaths)
                {
                    if (ImGui::MenuItem(line.c_str()))
                    {
                        Excella::activeDbPath = line;
                        reset_database();
                        load_database(Excella::activeDbPath.c_str());   // @HACK This should be threaded
                    }
                }

                ImGui::Separator();
                if (ImGui::MenuItem("Clear Recent Items"))
                {
                    Excella::cachedDbPaths.clear();
                    save_cache_file();
                }
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Quit", "CTRL+q")) Excella::quit = true;

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Entry"))
        {
            if (ImGui::MenuItem("Create entry", "CTRL+n"))
            {
                search.Clear();
                filter = {};
                create_entry();
                focusNewEntry = true;
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Tag Manager...")) showTagsPopup = true;

            if (ImGui::MenuItem("URL Manager...")) showUrlsPopup = true;

            ImGui::Separator();

            if (ImGui::MenuItem("Statistics...")) showStatisticsPopup = true;

            static constexpr const char* fmt = "Total Entries: %lu / %lu";
            static char buffer[std::char_traits<char>::length(fmt) + 256];
            sprintf(&buffer[0], fmt, Excella::actualTotalEntries, MAX_ENTRIES);
            ImGui::Text(buffer);

            ImGui::EndMenu();
        }

        if (showTagsPopup)
        {
            static bool popen = true;
            ImGui::OpenPopup("Tag Manager");

            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            int width  = 0;
            int height = 0;
            glfwGetWindowSize(Excella::window, &width, &height);
            ImGui::SetNextWindowSize({width / 3.0f, height / 2.0f}, ImGuiCond_FirstUseEver);

            if (ImGui::BeginPopupModal("Tag Manager", &popen))
            {
                if (populateCurrentTagsMap)
                {
                    currentTagsInDatabase.clear();
                    for (u64 i = 0; i < entryIdx; ++i)
                    {
                        for (const auto& tag : entries[i].tags) currentTagsInDatabase[tag]++;
                    }

                    populateCurrentTagsMap = false;
                }

                static ImGuiTextFilter filter;

                if (ImGui::Button("Clear")) filter.Clear();
                ImGui::SameLine();
                ImGui::PushItemWidth(-1);
                filter.Draw();
                ImGui::PopItemWidth();

                ImGui::Separator();

                static constexpr auto TABLE_FLAGS =
                    ImGuiTableFlags_Resizable |
                    ImGuiTableFlags_Sortable  |
                    ImGuiTableFlags_ScrollY   |
                    ImGuiTableFlags_ScrollX;

                static constexpr auto COLUMN_FLAGS = ImGuiTableColumnFlags_WidthFixed;

                if (ImGui::BeginTable("TableTags", 3, TABLE_FLAGS))
                {
                    ImGui::TableSetupColumn("Tag",       COLUMN_FLAGS, 300.0f);
                    ImGui::TableSetupColumn("Count",     COLUMN_FLAGS, 50.0f);
                    ImGui::TableSetupColumn("Purge",     COLUMN_FLAGS, 50.0f);

                    ImGui::TableSetupScrollFreeze(0, 1);
                    ImGui::TableHeadersRow();

                    for (const auto& kv : currentTagsInDatabase)
                    {
                        if (!filter.PassFilter(kv.first.c_str())) continue;

                        ImGui::TableNextColumn();
                        static constexpr auto SELECTABLE_FLAGS = ImGuiSelectableFlags_DontClosePopups;
                        bool selected = false;
                        if (ImGui::Selectable(kv.first.c_str(), &selected, SELECTABLE_FLAGS))
                            inclusiveTags.push_back(kv.first);
                        if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
                            exclusiveTags.push_back(kv.first);

                        ImGui::TableNextColumn();
                        char buffer[256];
                        sprintf(&buffer[0], "%i", kv.second);
                        ImGui::Text(buffer);

                        ImGui::TableNextColumn();
                        ImGui::PushID(&kv.first);
                        if (ImGui::Button("Purge"))
                        {
                            for (u64 i = 0; i < entryIdx; ++i)
                            {
                                auto begin = entries[i].tags.begin();
                                auto end   = entries[i].tags.end();

                                if (auto it = std::find(begin, end, kv.first); it != end) entries[i].tags.erase(it);
                            }

                            Excella::dirty = true;
                            populateCurrentTagsMap = true;
                        }
                        ImGui::PopID();
                    
                        ImGui::TableNextRow();
                    }

                    ImGui::EndTable();
                }

                ImGui::EndPopup();
            }

            if (!popen)
            {
                currentTagsInDatabase.clear();
                populateCurrentTagsMap = true;

                ImGui::CloseCurrentPopup();
                showTagsPopup = false;
                popen = true;
            }
        }

        if (showUrlsPopup)
        {
            ImGui::OpenPopup("URL Manager");

            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            int width  = 0;
            int height = 0;
            glfwGetWindowSize(Excella::window, &width, &height);
            ImGui::SetNextWindowSize({width / 2.0f, height / 2.0f}, ImGuiCond_FirstUseEver);

            if (ImGui::BeginPopupModal("URL Manager", &showUrlsPopup))
            {
                ImGui::Text("Total URLs: %lu", urls.size());
                ImGui::Text("Right click a URL to remove from database. Entries are updated on database save.");

                ImGui::Separator();

                static constexpr auto SELECTABLE_FLAGS = ImGuiSelectableFlags_DontClosePopups;
                for (const auto& kv : urls)
                {
                    bool selected = false;
                    ImGui::Selectable(kv.first.c_str(), &selected, SELECTABLE_FLAGS);
                    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
                    {
                        urls.erase(kv.first);
                        Excella::dirty = true;
                        break;  // @NOTE: We must break here to allow the for-loop's constraints to update.
                                // Otherwise, there will be an access violation.
                    }
                }

                ImGui::EndPopup();
            }
        }

        if (showStatisticsPopup)
        {
            ImGui::OpenPopup("Statistics");

            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            int width  = 0;
            int height = 0;
            glfwGetWindowSize(Excella::window, &width, &height);
            ImGui::SetNextWindowSize({width / 2.0f, height / 2.0f}, ImGuiCond_FirstUseEver);

            if (ImGui::BeginPopupModal("Statistics", &showStatisticsPopup))
            {
                if (ImGui::BeginTable("Stats", 2))
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("Total Entries");
                    ImGui::TableNextColumn();
                    ImGui::Text("%i / %i", entryIdx, MAX_ENTRIES);
                 
                    ImGui::EndTable();
                }

                ImGui::Separator();

                if (ImGui::TreeNodeEx("Completion"))
                {
                    if (ImGui::BeginTable("Details", 3))
                    {
                        u64 counts[COUNT_COMPLETION]{};

                        for (u64 i = 0; i < entryIdx; ++i) counts[entries[i].completion]++;

                        for (u64 completion = 0; completion < COUNT_COMPLETION; ++completion)
                        {
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text(completionStr[completion]);

                            ImGui::TableNextColumn();
                            ImGui::Text("%i", counts[completion]);

                            ImGui::TableNextColumn();
                            ImGui::Text("%.2f", (float)counts[completion] / entryIdx * 100);
                        }

                        ImGui::EndTable();
                    }

                    ImGui::TreePop();
                }

                if (ImGui::TreeNodeEx("Platform"))
                {
                    if (ImGui::BeginTable("Details", 3))
                    {
                        u64 counts[COUNT_PLATFORM]{};

                        for (u64 i = 0; i < entryIdx; ++i) counts[entries[i].platform]++;

                        for (u64 platform = 0; platform < COUNT_PLATFORM; ++platform)
                        {
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text(platformStr[platform]);

                            ImGui::TableNextColumn();
                            ImGui::Text("%i", counts[platform]);

                            ImGui::TableNextColumn();
                            ImGui::Text("%.2f", (float)counts[platform] / entryIdx * 100);
                        }

                        ImGui::EndTable();
                    }

                    ImGui::TreePop();
                }

                if (ImGui::TreeNodeEx("Release Year"))
                {
                    if (ImGui::BeginTable("Details", 3))
                    {
                        std::map<u64, u64> counts;

                        for (u64 i = 0; i < entryIdx; ++i) counts[entries[i].releaseYear]++;

                        for (const auto& kv : counts)
                        {
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("%i", kv.first);

                            ImGui::TableNextColumn();
                            ImGui::Text("%i", kv.second);

                            ImGui::TableNextColumn();
                            ImGui::Text("%.2f", (float)kv.second / entryIdx * 100);
                        }

                        ImGui::EndTable();
                    }

                    ImGui::TreePop();
                }

                if (ImGui::TreeNodeEx("Last Played Year"))
                {
                    if (ImGui::BeginTable("Details", 3))
                    {
                        std::map<u64, u64> counts;

                        for (u64 i = 0; i < entryIdx; ++i) counts[entries[i].lastPlayed]++;

                        for (const auto& kv : counts)
                        {
                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("%i", kv.first);

                            ImGui::TableNextColumn();
                            ImGui::Text("%i", kv.second);

                            ImGui::TableNextColumn();
                            ImGui::Text("%.2f", (float)kv.second / entryIdx * 100);
                        }

                        ImGui::EndTable();
                    }

                    ImGui::TreePop();
                }

                if (ImGui::TreeNodeEx("Tags"))
                {
                    static ImGuiTextFilter filter;
                    ImGui::PushItemWidth(-1);
                    filter.Draw();
                    ImGui::PopItemWidth();

                    if (ImGui::BeginTable("Details", 3))
                    {
                        std::map<std::string, u64> counts;

                        for (u64 i = 0; i < entryIdx; ++i)
                        {
                            for (const auto& tag : entries[i].tags)
                                counts[tag]++;
                        }


                        for (const auto& kv : counts)
                        {
                            if (!filter.PassFilter(kv.first.c_str())) continue;

                            ImGui::TableNextRow();
                            ImGui::TableNextColumn();
                            ImGui::Text("%s", kv.first.c_str());

                            ImGui::TableNextColumn();
                            ImGui::Text("%i", kv.second);

                            ImGui::TableNextColumn();
                            ImGui::Text("%.2f", (float)kv.second / entryIdx * 100);
                        }

                        ImGui::EndTable();
                    }

                    ImGui::TreePop();
                }

                ImGui::EndPopup();
            }
        }

        ImGui::Text("%s", Excella::activeDbPath.c_str());

        static Texture disketteRed  = load_texture_from_memory(&disketteRedBytes, disketteRedBytesSize);
        static Texture disketteGray = load_texture_from_memory(&disketteGrayBytes, disketteGrayBytesSize);
        Texture& diskette = Excella::dirty ? disketteRed : disketteGray;
        if (ImGui::ImageButton("", (void*)(intptr_t)diskette.data, ImVec2(18, 18))) save_database(Excella::activeDbPath.c_str());

        ImGui::EndMainMenuBar();
    }
}

void draw_file_browser()
{
    browser.Display();

    if (browser.HasSelected() && browserWantsSave)
    {
        std::string newPath = browser.GetSelected().string();
        Excella::activeDbPath = newPath.c_str();
        save_database(Excella::activeDbPath.c_str());
        browser.ClearSelected();
        reset_database();
        load_database(Excella::activeDbPath.c_str());   // @HACK This should be threaded
        browserWantsSave = false;
    }

    if (browser.HasSelected() && browserWantsLoad)
    {
        std::string newPath = browser.GetSelected().string();
        Excella::activeDbPath = newPath.c_str();
        reset_database();
        load_database(Excella::activeDbPath.c_str());   // @HACK this should be threaded
        browser.ClearSelected();
        browserWantsLoad = false;
    }
}

void draw_table()
{
    ImGui::SeparatorText("Search");
    ImGui::SameLine();
    if (ImGui::Button("Clear Search")) search.Clear();
    ImGui::SameLine();
    if (ImGui::Button("Clear All"))
    {
        filter = Filter{};
        search.Clear();
        inclusiveTags.clear();
        exclusiveTags.clear();
    }

    if (focusSearch) ImGui::SetKeyboardFocusHere();
    search.Draw("##On", -1.0f);

    // Filtering setup
    ImGuiTreeNodeFlags filterNodeFlags  = 0;
    if (filterNodeOpen) filterNodeFlags = ImGuiTreeNodeFlags_DefaultOpen;

    if (ImGui::TreeNodeEx("Filter", filterNodeFlags))
    {
        if (ImGui::BeginTable("Platforms", 17))
        {
            for (u64 platform = 0; platform < COUNT_PLATFORM; ++platform)
            {
                if (platform % 16 == 0) ImGui::TableNextRow();
                ImGui::TableNextColumn();
                if (ImGui::Checkbox(platformStr[platform], &filter.platformsSelected[platform])) filter.platformActive = true;
            }
            ImGui::EndTable();
        }
        ImGui::Separator();

        if (ImGui::InputInt("Release Year", &filter.releaseYear)) filter.releaseYearActive = true;
        if (ImGui::InputInt("Last Played", &filter.lastPlayed)) filter.lastPlayedActive = true;
        ImGui::Separator();

        if (ImGui::InputInt("Rating", &filter.rating)) filter.ratingActive = true;
        ImGui::Separator();

        if (ImGui::BeginTable("Completion", COUNT_COMPLETION))
        {
            for (u64 n = 0; n < COUNT_COMPLETION; ++n)
            {
                ImGui::TableNextColumn();
                if (ImGui::Checkbox(completionStr[n], &filter.completionsSelected[n])) filter.completionActive = true;
            }
            ImGui::EndTable();
        }
        ImGui::Separator();

        if (ImGui::Checkbox("S", &filter.sActive)) {}
        ImGui::SameLine();
        if (ImGui::Checkbox("j", &filter.jActive)) {}
        ImGui::SameLine();
        if (ImGui::Checkbox("t", &filter.tActive)) {}
        ImGui::Separator();

        // Filter by tag
        static std::string tagString;
        if (ImGui::InputText("##On", &tagString)) {}
        ImGui::SameLine();
        if (ImGui::Button("+Tag") && tagString.size() > 0)
        {
            inclusiveTags.push_back(tagString);
            tagString.clear();
        }
        ImGui::SameLine();
        if (ImGui::Button("-Tag") && tagString.size() > 0)
        {
            exclusiveTags.push_back(tagString);
            tagString.clear();
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear"))
        {
            inclusiveTags.clear();
            exclusiveTags.clear();
        }
        for (u64 tagIdx = 0; tagIdx < inclusiveTags.size(); ++tagIdx)
        {
            if (ImGui::Button(inclusiveTags[tagIdx].c_str())) inclusiveTags.erase(inclusiveTags.begin() + tagIdx);
            ImGui::SameLine();
        }
        for (u64 tagIdx = 0; tagIdx < exclusiveTags.size(); ++tagIdx)
        {
            static constexpr ImVec4 RED_COLOR{ 1.0, 0.2, 0.2, 1.0 };
            ImGui::PushStyleColor(ImGuiCol_Button, RED_COLOR);
            if (ImGui::Button(exclusiveTags[tagIdx].c_str())) exclusiveTags.erase(exclusiveTags.begin() + tagIdx);
            ImGui::PopStyleColor();
            ImGui::SameLine();
        }

        // Filter test
        bool test = false;
        for (u64 i = 0; i < COUNT_PLATFORM; ++i)
        {
            if (filter.platformsSelected[i]) test = true;
        }
        filter.platformActive = test;

        test = false;
        for (u64 i = 0; i < COUNT_COMPLETION; ++i)
        {
            if (filter.completionsSelected[i])
                test = true;
        }
        filter.completionActive = test;

        filter.active =
            filter.platformActive    ||
            filter.releaseYearActive ||
            filter.lastPlayedActive  ||
            filter.ratingActive      ||
            filter.completionActive  ||
            filter.sActive           ||
            filter.jActive           ||
            filter.tActive;

        ImGui::TreePop();
    }

    ImGui::Separator();
    static constexpr auto TABLE_FLAGS =
        ImGuiTableFlags_Resizable |
        ImGuiTableFlags_Sortable  |
        ImGuiTableFlags_ScrollY   |
        ImGuiTableFlags_ScrollX;

    ImGuiContext& g = *ImGui::GetCurrentContext();

    static constexpr ImVec2 CELL_PADDING(1.0f, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, CELL_PADDING);
    if (ImGui::BeginTable("Entries", 19, TABLE_FLAGS))
    {
        ImGuiTable* table = g.CurrentTable;

        // Column setup
        int width  = 0;
        int height = 0;
        glfwGetWindowSize(Excella::window, &width, &height);
        double k = width / 10;
        static constexpr auto COLUMN_FLAGS = ImGuiTableColumnFlags_WidthFixed;
        ImGui::TableSetupColumn("Title",            COLUMN_FLAGS,                                       k * 2.75);
        ImGui::TableSetupColumn("Sorting Title",    COLUMN_FLAGS | ImGuiTableColumnFlags_DefaultSort,   k / 2.0);
        ImGui::TableSetupColumn("Platform",         COLUMN_FLAGS,                                       k / 2.0);
        ImGui::TableSetupColumn("Region",           COLUMN_FLAGS,                                       k / 2.0);
        ImGui::TableSetupColumn("Release Year",     COLUMN_FLAGS,                                       k / 2.0);
        ImGui::TableSetupColumn("Update Status",    COLUMN_FLAGS | ImGuiTableColumnFlags_NoSort,        k / 1.6);
        ImGui::TableSetupColumn("Archived Version", COLUMN_FLAGS | ImGuiTableColumnFlags_NoSort,        k / 2.0);
        ImGui::TableSetupColumn("Best Version",     COLUMN_FLAGS | ImGuiTableColumnFlags_NoSort,        k / 2.0);
        ImGui::TableSetupColumn("DLC",              COLUMN_FLAGS | ImGuiTableColumnFlags_NoSort,        k / 1.6);
        ImGui::TableSetupColumn("Completion",       COLUMN_FLAGS,                                       k / 1.6);
        ImGui::TableSetupColumn("Rating",           COLUMN_FLAGS, 100.0);
        ImGui::TableSetupColumn("S",                COLUMN_FLAGS | ImGuiTableColumnFlags_NoSort, 25.0);
        ImGui::TableSetupColumn("J",                COLUMN_FLAGS | ImGuiTableColumnFlags_NoSort, 25.0);
        ImGui::TableSetupColumn("T",                COLUMN_FLAGS | ImGuiTableColumnFlags_NoSort, 25.0);
        ImGui::TableSetupColumn("Last Played",      COLUMN_FLAGS,                                       k / 2.0);
        ImGui::TableSetupColumn("T",                COLUMN_FLAGS | ImGuiTableColumnFlags_NoSort, 25.0);
        ImGui::TableSetupColumn("U",                COLUMN_FLAGS | ImGuiTableColumnFlags_NoSort, 25.0);
        ImGui::TableSetupColumn("N",                COLUMN_FLAGS | ImGuiTableColumnFlags_NoSort, 25.0);
        ImGui::TableSetupColumn("X",                COLUMN_FLAGS | ImGuiTableColumnFlags_NoSort, 25.0);

        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableHeadersRow();

        Excella::actualTotalEntries = 0;
        for (u64 i = 0; i < entryIdx; ++i)
        {
            if (entries[i].deleted) continue;

            ++Excella::actualTotalEntries;

            // Search (fuzzy filtering based on entry title)
            if (search.IsActive() && !search.PassFilter(entries[i].title.c_str())) continue;

            // Filtering
            if (filter.active)
            {
                if (filter.platformActive)
                {
                    bool shouldSkip = true;
                    for (u64 j = 0; j < COUNT_PLATFORM; ++j)
                    {
                        if (filter.platformsSelected[entries[i].platform]) shouldSkip = false;
                    }

                    if (shouldSkip) continue;
                }

                if (filter.releaseYearActive && entries[i].releaseYear != filter.releaseYear) continue;

                if (filter.lastPlayedActive && entries[i].lastPlayed != filter.lastPlayed) continue;

                if (filter.ratingActive && entries[i].rating != filter.rating) continue;

                if (filter.completionActive)
                {
                    bool shouldSkip = true;
                    for (u64 j = 0; j < COUNT_COMPLETION; ++j)
                    {
                        if (filter.completionsSelected[entries[i].completion]) shouldSkip = false;
                    }

                    if (shouldSkip) continue;
                }

                if (filter.sActive && !entries[i].s) continue;
                if (filter.jActive && !entries[i].j) continue;
                if (filter.tActive && !entries[i].t) continue;
            }

            // Filtering by tag
            auto breakLoop = false;
            if (!inclusiveTags.empty())
            {
                for (const auto& tag : inclusiveTags)
                {
                    if (std::find(entries[i].tags.begin(), entries[i].tags.end(), tag) == std::end(entries[i].tags))
                        breakLoop = true;
                }
            }
            if (breakLoop) continue;
            if (!exclusiveTags.empty())
            {
                for (const auto& tag : exclusiveTags)
                {
                    if (std::find(entries[i].tags.begin(), entries[i].tags.end(), tag) != std::end(entries[i].tags))
                        breakLoop = true;
                }
            }
            if (breakLoop) continue;

            ImGui::TableNextRow();

            if (focusNewEntry) ImGui::SetKeyboardFocusHere();

            ImGui::TableSetColumnIndex(0);
            ImGui::PushID(&entries[i].title);
            ImGui::PushItemWidth(-1);
            if (ImGui::InputTextWithHint("##On", "title", &entries[i].title)) Excella::dirty = true;
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&entries[i].sortingTitle);
            ImGui::PushItemWidth(-1);
            if (ImGui::InputTextWithHint("##On", "sorting title", &entries[i].sortingTitle)) Excella::dirty = true;
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&entries[i].platform);
            ImGui::PushItemWidth(-1);
            if (ImGui::BeginCombo("##On", platformStr[entries[i].platform]))
            {
                for (u64 n = 0; n < COUNT_PLATFORM; ++n)
                {
                    const bool isSelected = (entries[i].platform == n);
                    if (ImGui::Selectable(platformStr[n], isSelected))
                    {
                        entries[i].platform = (Platform)n;
                        Excella::dirty = true;
                    }

                    if (isSelected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&entries[i].region);
            ImGui::PushItemWidth(-1);
            if (ImGui::BeginCombo("##On", regionStr[entries[i].region]))
            {
                for (u64 n = 0; n < COUNT_REGION; ++n)
                {
                    const bool isSelected = (entries[i].region == n);
                    if (ImGui::Selectable(regionStr[n], isSelected))
                    {
                        entries[i].region = (Region)n;
                        Excella::dirty = true;
                    }

                    if (isSelected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&entries[i].releaseYear);
            ImGui::PushItemWidth(-1);
            if (ImGui::InputInt("##On", &entries[i].releaseYear)) Excella::dirty = true;
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&entries[i].updateStatus);
            ImGui::PushItemWidth(-1);
            if (ImGui::BeginCombo("##On", contentStatusStr[entries[i].updateStatus]))
            {
                for (u64 n = 0; n < COUNT_CONTENT_STATUS; ++n)
                {
                    const bool isSelected = (entries[i].updateStatus == n);
                    if (ImGui::Selectable(contentStatusStr[n], isSelected))
                    {
                        entries[i].updateStatus = (ContentStatus)n;
                        Excella::dirty = true;
                    }

                    if (isSelected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&entries[i].archivedVersion);
            ImGui::PushItemWidth(-1);
            if (ImGui::InputTextWithHint("##On", "archived version", &entries[i].archivedVersion)) Excella::dirty = true;
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&entries[i].bestVersion);
            ImGui::PushItemWidth(-1);
            if (ImGui::InputTextWithHint("##On", "known best version", &entries[i].bestVersion)) Excella::dirty = true;
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&entries[i].dlcStatus);
            ImGui::PushItemWidth(-1);
            if (ImGui::BeginCombo("##On", contentStatusStr[entries[i].dlcStatus]))
            {
                for (u64 n = 0; n < 5; ++n)
                {
                    const bool isSelected = (entries[i].dlcStatus == n);
                    if (ImGui::Selectable(contentStatusStr[n], isSelected))
                    {
                        entries[i].dlcStatus = (ContentStatus)n;
                        Excella::dirty = true;
                    }

                    if (isSelected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&entries[i].completion);
            ImGui::PushItemWidth(-1);
            if (ImGui::BeginCombo("##On", completionStr[entries[i].completion]))
            {
                for (u64 n = 0; n < COUNT_COMPLETION; ++n)
                {
                    const bool isSelected = (entries[i].completion == n);
                    if (ImGui::Selectable(completionStr[n], isSelected))
                    {
                        entries[i].completion = (Completion)n;
                        Excella::dirty = true;
                    }

                    if (isSelected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&entries[i].rating);
            ImGui::PushItemWidth(-1);
            if (ImGui::InputInt("##On", &entries[i].rating)) Excella::dirty = true;
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&entries[i].s);
            ImGui::PushItemWidth(-1);
            if (ImGui::Checkbox("##On", &entries[i].s)) Excella::dirty = true;
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&entries[i].j);
            ImGui::PushItemWidth(-1);
            if (ImGui::Checkbox("##On", &entries[i].j)) Excella::dirty = true;
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&entries[i].t);
            ImGui::PushItemWidth(-1);
            if (ImGui::Checkbox("##On", &entries[i].t)) Excella::dirty = true;
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&entries[i].lastPlayed);
            ImGui::PushItemWidth(-1);
            if (ImGui::InputInt("##On", &entries[i].lastPlayed)) Excella::dirty = true;
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            // Tags
            ImGui::TableNextColumn();
            ImGui::PushID(&entries[i].tags);
            ImGui::PushItemWidth(-1);
            static Texture tagsBlack = load_texture_from_memory(&tagBlackBytes, tagBlackBytesSize);
            static Texture tagsBlue = load_texture_from_memory(&tagGreenBytes, tagGreenBytesSize);
            Texture* useTags = entries[i].tags.size() == 0 ? &tagsBlack : &tagsBlue;
            if (ImGui::ImageButton("", (void*)(intptr_t)useTags->data, ImVec2(18, 18))) ImGui::OpenPopup("Edit Tags");
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            if (ImGui::BeginPopupModal("Edit Tags", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                static std::string newTag;
                ImGui::InputText("##On", &newTag);
                ImGui::SameLine();
                if (ImGui::Button("Add") && newTag.size() > 0)
                {
                    entries[i].tags.push_back(newTag);
                    newTag.clear();
                    Excella::dirty = true;
                }

                static constexpr auto SELECTABLE_FLAGS = ImGuiSelectableFlags_DontClosePopups;

                for (u64 tagIdx = 0; tagIdx < entries[i].tags.size(); ++tagIdx)
                {
                    bool selected = false;
                    if (ImGui::Selectable(entries[i].tags[tagIdx].c_str(), &selected, SELECTABLE_FLAGS))
                    {
                        entries[i].tags.erase(entries[i].tags.begin() + tagIdx);
                        Excella::dirty = true;
                    }
                }

                if (ImGui::Button("Close"))
                {
                    newTag.clear();
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            // URLs
            ImGui::TableNextColumn();
            ImGui::PushID(&entries[i].hasUrl);
            ImGui::PushItemWidth(-1);
            static Texture url = load_texture_from_memory(&urlBytes, urlBytesSize);
            if (ImGui::ImageButton("", (void*)(intptr_t)url.data, ImVec2(18, 18))) ImGui::OpenPopup("Edit URLs");
            ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            if (ImGui::BeginPopupModal("Edit URLs", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                static std::string newUrl;
                ImGui::InputText("New URL", &newUrl);
                ImGui::SameLine();
                if (ImGui::Button("Add"))
                {
                    if (newUrl.length() > 0)
                    {
                        urls[newUrl].push_back(entries[i].sortingTitle);    // @HACK: This should use uuid
                        newUrl.clear();
                        Excella::dirty = true;
                    }
                }

                ImGui::SeparatorText("URLs");

                static constexpr auto SELECTABLE_FLAGS = ImGuiSelectableFlags_DontClosePopups;
                for (auto& kv : urls)
                {
                    for (auto& title : kv.second)
                    {
                        bool selected = false;
                        if (title == entries[i].sortingTitle)
                        {
                            ImGui::Selectable(kv.first.c_str(), &selected, SELECTABLE_FLAGS);
                            if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                            {
#ifdef OS_WINDOWS
                                ShellExecuteA(
                                    NULL,
                                    "open",
                                    "C:\\Program Files\\LibreWolf\\librewolf.exe",  // @HACK obvious...
                                    kv.first.c_str(),
                                    NULL,
                                    SW_SHOWDEFAULT);
#endif
                            }

                            else if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
                            {
                                auto it = std::find(
                                    kv.second.begin(),
                                    kv.second.end(),
                                    entries[i].sortingTitle
                                );

                                kv.second.erase(it);
                                Excella::dirty = true;
                            }
                        }
                    }
                }

                if (ImGui::Button("Close")) ImGui::CloseCurrentPopup(); // @HACK: This might need to do more.

                ImGui::EndPopup();
            }
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            // Note
            ImGui::TableNextColumn();
            ImGui::PushID(&entries[i].notes);
            ImGui::PushItemWidth(-1);
            static Texture edit = load_texture_from_memory(&editBytes, editBytesSize);
            static Texture editWhite = load_texture_from_memory(&editWhiteBytes, editWhiteBytesSize);
            Texture* useEdit = entries[i].notes.size() == 0 ? &edit : &editWhite;
            if (ImGui::ImageButton("", (void*)(intptr_t)useEdit->data, ImVec2(18, 18))) ImGui::OpenPopup("Edit Notes");
            center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            if (ImGui::BeginPopupModal("Edit Notes", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                if (ImGui::InputTextMultiline("##On", &entries[i].notes, ImVec2(500, 500))) Excella::dirty = true;
                if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            // Delete button
            ImGui::TableNextColumn();
            ImGui::PushID(&entries[i].deleted);
            ImGui::PushItemWidth(-1);
            static Texture trashcan = load_texture_from_memory(&trashcanBytes, trashcanBytesSize);
            if (ImGui::ImageButton("", (void*)(intptr_t)trashcan.data, ImVec2(18, 18)))
            {
                entries[i].deleted = true;
                --Excella::actualTotalEntries;
                Excella::dirty = true;
            }
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            if (focusNewEntry) ImGui::SetScrollHereY(0.5f); // @BUG Why is this grayed-out?

            // Highlight entries based on archive status
            static constexpr auto orange = ImVec4(0.6f, 0.4f, 0.2f, 1.0f);
            static constexpr auto red    = ImVec4(0.6f, 0.2f, 0.2f, 1.0f);
            static constexpr auto yellow = ImVec4(0.6f, 0.6f, 0.0f, 1.0f);
            static constexpr auto green  = ImVec4(0.2f, 0.6f, 0.4f, 1.0f);

            auto& e = entries[i];
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

            bool shouldHighlightGreen = (e.dlcStatus == DOWNLOADED || e.dlcStatus == CS_NONE) &&
                (e.updateStatus == DOWNLOADED || e.updateStatus == LOCKED) &&
                (e.completion == COMPLETED);
            if (shouldHighlightGreen) table->RowBgColor[1] = ImGui::GetColorU32(green);

            // Highlighting hovered row
            ImGui::TableSetColumnIndex(table->Columns.size() - 1); // Jump to last column in case not enough has been drawn

            static ImRect rowRect(table->WorkRect.Min.x, table->RowPosY1, table->WorkRect.Max.x, table->RowPosY2);

            rowRect.Min.x = table->WorkRect.Min.x;
            rowRect.Min.y = table->RowPosY1;
            rowRect.Max.x = table->WorkRect.Max.x;
            rowRect.Max.y = table->RowPosY2;

            rowRect.ClipWith(table->BgClipRect);

            bool hover = ImGui::IsMouseHoveringRect(rowRect.Min, rowRect.Max, false) &&
                ImGui::IsWindowHovered(ImGuiHoveredFlags_None);
            // !ImGui::IsAnyItemHovered(); This can optionally disable row highlighting if any cells are hovered.

            if (hover) table->RowBgColor[1] = ImGui::GetColorU32(ImGuiCol_Border);
        }

        // Sorting table entries
        if (auto* sortSpecs = ImGui::TableGetSortSpecs())
        {
            if (sortSpecs->SpecsDirty)
            {
                std::sort(
                    &entries[0], &entries[entryIdx],
                    [&sortSpecs](const Entry& lhs, const Entry& rhs) -> bool {
                        for (i32 i = 0; i < sortSpecs->SpecsCount; ++i)     // i here is always 1...
                        {
                            const ImGuiTableColumnSortSpecs* currentSpecs = &sortSpecs->Specs[i];
                            bool sort = false;
                            switch (currentSpecs->ColumnIndex)
                            {
                                // Title
                            case 0: {
                                auto numeric = lhs.title.compare(rhs.title);
                                if (numeric == 0) return false;
                                if (currentSpecs->SortDirection == ImGuiSortDirection_Ascending) return numeric < 0;
                                return numeric > 0;
                            }; break;

                                // Sorting Title
                            case 1: {
                                auto numeric = lhs.sortingTitle.compare(rhs.sortingTitle);
                                if (numeric == 0) return false;
                                if (currentSpecs->SortDirection == ImGuiSortDirection_Ascending) return numeric < 0;
                                return numeric > 0;
                            }; break;

                                // Platform
                            case 2: {
                                if (currentSpecs->SortDirection == ImGuiSortDirection_Ascending)
                                {
                                    return lhs.platform < rhs.platform;
                                }
                                else
                                {
                                    if (lhs.platform > rhs.platform || rhs.platform < lhs.platform)
                                        return true;

                                    return false;
                                }
                            }; break;

                                // Region
                            case 3: {
                                if (currentSpecs->SortDirection == ImGuiSortDirection_Ascending)
                                {
                                    return lhs.region < rhs.region;
                                }
                                else
                                {
                                    if (lhs.region > rhs.region || rhs.region < lhs.region)
                                        return true;

                                    return false;
                                }
                            }; break;

                                // Release year
                            case 4: {
                                if (currentSpecs->SortDirection == ImGuiSortDirection_Ascending)
                                {
                                    return lhs.releaseYear < rhs.releaseYear;
                                }
                                else
                                {
                                    if (lhs.releaseYear > rhs.releaseYear || rhs.releaseYear < lhs.releaseYear)
                                        return true;

                                    return false;
                                }
                            }; break;

                                // Completion
                            case 9: {
                                if (currentSpecs->SortDirection == ImGuiSortDirection_Ascending)
                                {
                                    return lhs.completion < rhs.completion;
                                }
                                else
                                {
                                    if (lhs.completion > rhs.completion || rhs.completion < lhs.completion)
                                        return true;

                                    return false;
                                }
                            }; break;

                                // Rating
                            case 10: {
                                if (currentSpecs->SortDirection == ImGuiSortDirection_Ascending) return lhs.rating < rhs.rating;
                                if (lhs.rating == rhs.rating) return false;
                                return lhs.rating > rhs.rating;
                            }; break;

                                // S
                                // J
                                // T

                                // Last played
                            case 14: {
                                if (currentSpecs->SortDirection == ImGuiSortDirection_Ascending)
                                {
                                    return lhs.lastPlayed < rhs.lastPlayed;
                                }
                                else
                                {
                                    if (lhs.lastPlayed > rhs.lastPlayed || rhs.lastPlayed < lhs.lastPlayed)
                                        return true;

                                    return false;
                                }
                            }; break;

                            default: {
                                assert(false && "Something fucky this way comes");
                                return false;
                            }; break;
                            }
                            return currentSpecs->SortDirection == ImGuiSortDirection_Ascending ? sort : !sort;
                        }
                        assert(false && "Something fucky this way comes");
                        return false;
                    });

                sortSpecs->SpecsDirty = false;
            }
        }
        ImGui::EndTable();
    }
    ImGui::PopStyleVar();
}
