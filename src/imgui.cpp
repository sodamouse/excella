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
ImGuiTreeNodeFlags filterNodeFlags;

// Tags
static std::vector<std::string> activeTags;

// Tag manager
static std::map<std::string, i32> currentTagsInDatabase;
static bool showTagsPopup = false;
static bool populateCurrentTagsMap = true;

// urls
static bool showUrlsPopup = false;

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
    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_Q)) glfwSetWindowShouldClose(Excella::window, true);

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

    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_F)) focusSearch = true;

    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_G))
    {
        filterNodeOpen = !filterNodeOpen;
        if (filterNodeOpen) filterNodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;
        else filterNodeFlags = {};
    }

    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_T))
    {
        showTagsPopup = !showTagsPopup;
        if (!showTagsPopup)
        {
            populateCurrentTagsMap = true;
            currentTagsInDatabase.clear();
            ImGui::CloseCurrentPopup();
        }
    }
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
                        load_database(Excella::activeDbPath.c_str());
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

            if (ImGui::MenuItem("Quit", "CTRL+q"))  glfwSetWindowShouldClose(Excella::window, true);

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

            if (ImGui::MenuItem("URL Manager")) showUrlsPopup = true;

            ImGui::Separator();

            static constexpr const char* fmt = "Total Entries: %lu / %lu";
            static char buffer[std::char_traits<char>::length(fmt) + 256];
            sprintf(&buffer[0], fmt, Excella::actualTotalEntries, ENTRIES_MAX);
            ImGui::Text(buffer);

            ImGui::EndMenu();
        }

        if (showTagsPopup)
        {
            ImGui::OpenPopup("Tag Manager");
            
            if (ImGui::BeginPopupModal("Tag Manager"))
            {
                if (populateCurrentTagsMap)
                {
                    for (u64 i = 0; i < entryIdx; ++i)
                    {
                        for (const auto& tag : ENTRIES[i].tags) currentTagsInDatabase[tag]++;
                    }

                    populateCurrentTagsMap = false;
                }

                if (ImGui::BeginTable("Tags", 2))
                {
                    for (const auto& kv : currentTagsInDatabase)
                    {
                        ImGui::TableNextColumn();
                        static constexpr auto SELECTABLE_FLAGS = ImGuiSelectableFlags_DontClosePopups;
                        bool selected = false;
                        if (ImGui::Selectable(kv.first.c_str(), &selected, SELECTABLE_FLAGS))
                            activeTags.push_back(kv.first);
                    
                        ImGui::TableNextColumn();
                        char buffer[256];
                        sprintf(&buffer[0], "%i", kv.second);
                        ImGui::Text(buffer);
                    
                        ImGui::TableNextRow();
                    }

                    ImGui::EndTable();
                }

                if (ImGui::Button("Close"))
                {
                    populateCurrentTagsMap = true;
                    currentTagsInDatabase.clear();

                    showTagsPopup = false;

                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }
        }

        if (showUrlsPopup)
        {
            ImGui::OpenPopup("URL Manager");

            if (ImGui::BeginPopupModal("URL Manager"))
            {
                static std::string newUrl;
                ImGui::InputText("New URL", &newUrl);
                ImGui::SameLine();
                
                if (ImGui::Button("Add")) urls[newUrl];

                static constexpr auto SELECTABLE_FLAGS = ImGuiSelectableFlags_DontClosePopups;
                for (const auto& kv : urls)
                {
                    bool selected = false;
                    ImGui::Selectable(kv.first.c_str(), &selected, SELECTABLE_FLAGS);
                    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
                    {
                        urls.erase(kv.first);
                        Excella::dirty = true;
                        break;  // We must break here to allow the for-loop's constraints to update.
                                // Otherwise, there will be an access violation.
                    }
                }

                if (ImGui::Button("Close"))
                {
                    showUrlsPopup = false;

                    ImGui::CloseCurrentPopup();
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
        load_database(Excella::activeDbPath.c_str());
        browserWantsSave = false;
    }

    if (browser.HasSelected() && browserWantsLoad)
    {
        std::string newPath = browser.GetSelected().string();
        Excella::activeDbPath = newPath.c_str();
        reset_database();
        load_database(Excella::activeDbPath.c_str());
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
        activeTags.clear();
    }

    if (focusSearch) ImGui::SetKeyboardFocusHere();
    search.Draw("##On", -1.0f);

    // Filtering setup
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
        if (ImGui::Button("Add Tag Filter") && tagString.size() > 0)
        {
            activeTags.push_back(tagString);
            tagString.clear();
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear"))
        {
            activeTags.clear();
        }
        for (u64 tagIdx = 0; tagIdx < activeTags.size(); ++tagIdx)
        {
            if (ImGui::Button(activeTags[tagIdx].c_str())) activeTags.erase(activeTags.begin() + tagIdx);
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
        ImGuiTableFlags_ScrollY;

    ImGuiContext& g = *ImGui::GetCurrentContext();

    static constexpr ImVec2 CELL_PADDING(1.0f, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, CELL_PADDING);
    if (ImGui::BeginTable("Entries", 19, TABLE_FLAGS))
    {
        ImGuiTable* table = g.CurrentTable;

        static auto COLUMN_FLAGS = ImGuiTableColumnFlags_WidthFixed;
        ImGui::TableSetupColumn("Title", COLUMN_FLAGS, 475.0);
        ImGui::TableSetupColumn("Sorting Title", ImGuiTableColumnFlags_DefaultSort);
        ImGui::TableSetupColumn("Platform");
        ImGui::TableSetupColumn("Region", COLUMN_FLAGS, 50.0);
        ImGui::TableSetupColumn("Release Year");
        ImGui::TableSetupColumn("Update Status", ImGuiTableColumnFlags_NoSort);
        ImGui::TableSetupColumn("Archived Version", ImGuiTableColumnFlags_NoSort);
        ImGui::TableSetupColumn("Best Version", ImGuiTableColumnFlags_NoSort);
        ImGui::TableSetupColumn("DLC", ImGuiTableColumnFlags_NoSort);
        ImGui::TableSetupColumn("Completion");
        ImGui::TableSetupColumn("Rating");
        ImGui::TableSetupColumn("S", COLUMN_FLAGS | ImGuiTableColumnFlags_NoSort, 25.0);
        ImGui::TableSetupColumn("J", COLUMN_FLAGS | ImGuiTableColumnFlags_NoSort, 25.0);
        ImGui::TableSetupColumn("T", COLUMN_FLAGS | ImGuiTableColumnFlags_NoSort, 25.0);
        ImGui::TableSetupColumn("Last Played");
        ImGui::TableSetupColumn("T", COLUMN_FLAGS | ImGuiTableColumnFlags_NoSort, 25.0);
        ImGui::TableSetupColumn("U", COLUMN_FLAGS | ImGuiTableColumnFlags_NoSort, 25.0);
        ImGui::TableSetupColumn("N", COLUMN_FLAGS | ImGuiTableColumnFlags_NoSort, 25.0);
        ImGui::TableSetupColumn("X", COLUMN_FLAGS | ImGuiTableColumnFlags_NoSort, 25.0);

        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableHeadersRow();

        Excella::actualTotalEntries = 0;
        for (u64 i = 0; i < entryIdx; ++i)
        {
            if (ENTRIES[i].deleted) continue;

            ++Excella::actualTotalEntries;

            // Search (fuzzy filtering based on entry title)
            if (search.IsActive() && !search.PassFilter(ENTRIES[i].title.c_str())) continue;

            // Filtering
            if (filter.active)
            {
                if (filter.platformActive)
                {
                    bool shouldSkip = true;
                    for (u64 j = 0; j < COUNT_PLATFORM; ++j)
                    {
                        if (filter.platformsSelected[ENTRIES[i].platform]) shouldSkip = false;
                    }

                    if (shouldSkip) continue;
                }

                if (filter.releaseYearActive && ENTRIES[i].releaseYear != filter.releaseYear) continue;

                if (filter.lastPlayedActive && ENTRIES[i].lastPlayed != filter.lastPlayed) continue;

                if (filter.ratingActive && ENTRIES[i].rating != filter.rating) continue;

                if (filter.completionActive)
                {
                    bool shouldSkip = true;
                    for (u64 j = 0; j < COUNT_COMPLETION; ++j)
                    {
                        if (filter.completionsSelected[ENTRIES[i].completion]) shouldSkip = false;
                    }

                    if (shouldSkip) continue;
                }

                if (filter.sActive && !ENTRIES[i].s) continue;
                if (filter.jActive && !ENTRIES[i].j) continue;
                if (filter.tActive && !ENTRIES[i].t) continue;
            }

            // Filtering by tag
            auto breakLoop = false;
            if (!activeTags.empty())
            {
                for (const auto& tag : activeTags)
                {
                    if (std::find(ENTRIES[i].tags.begin(), ENTRIES[i].tags.end(), tag) == std::end(ENTRIES[i].tags))
                        breakLoop = true;
                }
            }
            if (breakLoop) continue;

            ImGui::TableNextRow();

            if (focusNewEntry) ImGui::SetKeyboardFocusHere();

            ImGui::TableSetColumnIndex(0);
            ImGui::PushID(&ENTRIES[i].title);
            ImGui::PushItemWidth(-1);
            if (ImGui::InputTextWithHint("##On", "title", &ENTRIES[i].title)) Excella::dirty = true;
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].sortingTitle);
            ImGui::PushItemWidth(-1);
            if (ImGui::InputTextWithHint("##On", "sorting title", &ENTRIES[i].sortingTitle)) Excella::dirty = true;
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].platform);
            ImGui::PushItemWidth(-1);
            if (ImGui::BeginCombo("##On", platformStr[ENTRIES[i].platform]))
            {
                for (u64 n = 0; n < COUNT_PLATFORM; ++n)
                {
                    const bool isSelected = (ENTRIES[i].platform == n);
                    if (ImGui::Selectable(platformStr[n], isSelected))
                    {
                        ENTRIES[i].platform = (Platform)n;
                        Excella::dirty = true;
                    }

                    if (isSelected) ImGui::SetItemDefaultFocus();
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
                for (u64 n = 0; n < COUNT_REGION; ++n)
                {
                    const bool isSelected = (ENTRIES[i].region == n);
                    if (ImGui::Selectable(regionStr[n], isSelected))
                    {
                        ENTRIES[i].region = (Region)n;
                        Excella::dirty = true;
                    }

                    if (isSelected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].releaseYear);
            ImGui::PushItemWidth(-1);
            if (ImGui::InputInt("##On", &ENTRIES[i].releaseYear)) Excella::dirty = true;
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].updateStatus);
            ImGui::PushItemWidth(-1);
            if (ImGui::BeginCombo("##On", contentStatusStr[ENTRIES[i].updateStatus]))
            {
                for (u64 n = 0; n < COUNT_CONTENT_STATUS; ++n)
                {
                    const bool isSelected = (ENTRIES[i].updateStatus == n);
                    if (ImGui::Selectable(contentStatusStr[n], isSelected))
                    {
                        ENTRIES[i].updateStatus = (ContentStatus)n;
                        Excella::dirty = true;
                    }

                    if (isSelected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].archivedVersion);
            ImGui::PushItemWidth(-1);
            if (ImGui::InputTextWithHint("##On", "archived version", &ENTRIES[i].archivedVersion)) Excella::dirty = true;
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].bestVersion);
            ImGui::PushItemWidth(-1);
            if (ImGui::InputTextWithHint("##On", "known best version", &ENTRIES[i].bestVersion)) Excella::dirty = true;
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].dlcStatus);
            ImGui::PushItemWidth(-1);
            if (ImGui::BeginCombo("##On", contentStatusStr[ENTRIES[i].dlcStatus]))
            {
                for (u64 n = 0; n < 5; ++n)
                {
                    const bool isSelected = (ENTRIES[i].dlcStatus == n);
                    if (ImGui::Selectable(contentStatusStr[n], isSelected))
                    {
                        ENTRIES[i].dlcStatus = (ContentStatus)n;
                        Excella::dirty = true;
                    }

                    if (isSelected) ImGui::SetItemDefaultFocus();
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
                for (u64 n = 0; n < COUNT_COMPLETION; ++n)
                {
                    const bool isSelected = (ENTRIES[i].completion == n);
                    if (ImGui::Selectable(completionStr[n], isSelected))
                    {
                        ENTRIES[i].completion = (Completion)n;
                        Excella::dirty = true;
                    }

                    if (isSelected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].rating);
            ImGui::PushItemWidth(-1);
            if (ImGui::InputInt("##On", &ENTRIES[i].rating)) Excella::dirty = true;
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].s);
            ImGui::PushItemWidth(-1);
            if (ImGui::Checkbox("##On", &ENTRIES[i].s)) Excella::dirty = true;
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].j);
            ImGui::PushItemWidth(-1);
            if (ImGui::Checkbox("##On", &ENTRIES[i].j)) Excella::dirty = true;
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].t);
            ImGui::PushItemWidth(-1);
            if (ImGui::Checkbox("##On", &ENTRIES[i].t)) Excella::dirty = true;
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].lastPlayed);
            ImGui::PushItemWidth(-1);
            if (ImGui::InputInt("##On", &ENTRIES[i].lastPlayed)) Excella::dirty = true;
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            // Tags
            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].tags);
            ImGui::PushItemWidth(-1);
            static Texture tagsBlack = load_texture_from_memory(&tagBlackBytes, tagBlackBytesSize);
            static Texture tagsBlue = load_texture_from_memory(&tagGreenBytes, tagGreenBytesSize);
            Texture* useTags = ENTRIES[i].tags.size() == 0 ? &tagsBlack : &tagsBlue;
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
                    ENTRIES[i].tags.push_back(newTag);
                    newTag.clear();
                    Excella::dirty = true;
                }

                static constexpr auto SELECTABLE_FLAGS = ImGuiSelectableFlags_DontClosePopups;

                for (u64 tagIdx = 0; tagIdx < ENTRIES[i].tags.size(); ++tagIdx)
                {
                    bool selected = false;
                    if (ImGui::Selectable(ENTRIES[i].tags[tagIdx].c_str(), &selected, SELECTABLE_FLAGS))
                    {
                        ENTRIES[i].tags.erase(ENTRIES[i].tags.begin() + tagIdx);
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
            ImGui::PushID(&ENTRIES[i].hasUrl);
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
                        urls[newUrl].push_back(ENTRIES[i].sortingTitle);    // @HACK: This should use uuid
                        newUrl.clear();
                        Excella::dirty = true;
                    }
                }

                ImGui::SeparatorText("Current URLs");

                static constexpr auto SELECTABLE_FLAGS = ImGuiSelectableFlags_DontClosePopups;
                for (auto& kv : urls)
                {
                    for (auto& title : kv.second)
                    {
                        bool selected = false;
                        if (title == ENTRIES[i].sortingTitle)
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
                                    ENTRIES[i].sortingTitle
                                );

                                kv.second.erase(it);
                                Excella::dirty = true;
                            }
                        }
                    }
                }

                ImGui::SeparatorText("All URLs");

                for (auto& kv : urls)
                {
                    bool selected = false;
                    if (ImGui::Selectable(kv.first.c_str(), &selected, SELECTABLE_FLAGS))
                    {
                        kv.second.push_back(ENTRIES[i].sortingTitle);   // @HACK: This shoud use uuid.
                        Excella::dirty = true;
                    }
                }

                if (ImGui::Button("Close")) ImGui::CloseCurrentPopup(); // @HACK: This might need to do more.

                ImGui::EndPopup();
            }
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            // Note
            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].notes);
            ImGui::PushItemWidth(-1);
            static Texture edit = load_texture_from_memory(&editBytes, editBytesSize);
            static Texture editWhite = load_texture_from_memory(&editWhiteBytes, editWhiteBytesSize);
            Texture* useEdit = ENTRIES[i].notes.size() == 0 ? &edit : &editWhite;
            if (ImGui::ImageButton("", (void*)(intptr_t)useEdit->data, ImVec2(18, 18))) ImGui::OpenPopup("Edit Notes");
            center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            if (ImGui::BeginPopupModal("Edit Notes", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                if (ImGui::InputTextMultiline("##On", &ENTRIES[i].notes, ImVec2(500, 500))) Excella::dirty = true;
                if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            // Delete button
            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].deleted);
            ImGui::PushItemWidth(-1);
            static Texture trashcan = load_texture_from_memory(&trashcanBytes, trashcanBytesSize);
            if (ImGui::ImageButton("", (void*)(intptr_t)trashcan.data, ImVec2(18, 18)))
            {
                ENTRIES[i].deleted = true;
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
                    &ENTRIES[0], &ENTRIES[entryIdx],
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
