#include "imgui.hpp"
#include "core.hpp"
#include "database.hpp"
#include "entry.hpp"
#include "excella.hpp"
#include "texture.hpp"

#include "file_browser/filebrowser.hpp"
#include <GLFW/glfw3.h>

#include <algorithm>
#include <fstream>

static ImGuiTextFilter search;

struct Filter
{
    bool active = false;

    bool platformsSelected[ARRAY_SZ(platformStr)] {};
    bool platformActive = false;

    i32 releaseYear = 0;
    bool releaseYearActive = false;

    i32 lastPlayed = 0;
    bool lastPlayedActive = false;

    i32 rating = 0;
    bool ratingActive = false;

    bool completionsSelected[ARRAY_SZ(completionStr)] {};
    bool completionActive = false;

    bool sActive = false;
    bool jActive = false;
    bool tActive = false;
};

static Filter filter {};

void draw_table(bool focusSearch, bool focusNewEntry)
{
    ImGui::SeparatorText("Search");
    ImGui::SameLine();
    if (ImGui::Button("Clear Search"))
    {
        search.Clear();
    }
    ImGui::SameLine();
    if (ImGui::Button("Clear All"))
    {
        filter = Filter {};
        search.Clear();
    }

    if (focusSearch)
        ImGui::SetKeyboardFocusHere();
    search.Draw("##On", -1.0f);

    // Filtering setup
    if (ImGui::TreeNode("Filter"))
    {
        if (ImGui::BeginTable("Platforms", ARRAY_SZ(platformStr))) // TODO this should be changed to correct number of columns
        {
            for (u64 n = 0; n < ARRAY_SZ(platformStr); ++n)
            {
                if (n % 16 == 0) ImGui::TableNextRow();
                ImGui::TableNextColumn();

                if (ImGui::Checkbox(platformStr[n], &filter.platformsSelected[n]))
                {
                    filter.platformActive = true;
                }
            }
            ImGui::EndTable();
        }
        ImGui::Separator();

        if (ImGui::InputInt("Release Year", &filter.releaseYear))
        {
            filter.releaseYearActive = true;
        }

        if (ImGui::InputInt("Last Played", &filter.lastPlayed))
        {
            filter.lastPlayedActive = true;
        }
        ImGui::Separator();

        if (ImGui::InputInt("Rating", &filter.rating))
        {
            filter.ratingActive = true;
        }
        ImGui::Separator();

        if (ImGui::BeginTable("Completion", ARRAY_SZ(completionStr))) // TODO this should be changed to correct no of columns
        {
            for (u64 n = 0; n < ARRAY_SZ(completionStr); ++n)
            {
                if (n % 5 == 0) ImGui::TableNextRow();
                ImGui::TableNextColumn();

                if (ImGui::Checkbox(completionStr[n], &filter.completionsSelected[n]))
                {
                    filter.completionActive = true;
                }
            }
            ImGui::EndTable();
        }
        ImGui::Separator();

        // clang-format off
        if (ImGui::Checkbox("S", &filter.sActive))
            ;
        ImGui::SameLine();
        if (ImGui::Checkbox("j", &filter.jActive))
            ;
        ImGui::SameLine();
        if (ImGui::Checkbox("t", &filter.tActive))
            ;
        // clang-format on
        ImGui::Separator();

        // Filter test
        bool test = false;
        for (u64 i = 0; i < ARRAY_SZ(platformStr); ++i)
        {
            if (filter.platformsSelected[i])
                test = true;
        }
        filter.platformActive = test;

        test = false;
        for (u64 i = 0; i < ARRAY_SZ(completionStr); ++i)
        {
            if (filter.completionsSelected[i])
                test = true;
        }
        filter.completionActive = test;

        // clang-format off
        filter.active =
            filter.platformActive    ||
            filter.releaseYearActive ||
            filter.lastPlayedActive  ||
            filter.ratingActive      ||
            filter.completionActive  ||
            filter.sActive           ||
            filter.jActive           ||
            filter.tActive;
        // clang-format on

        ImGui::TreePop();
    }

    ImGui::Separator();
    static auto flags =
        ImGuiTableFlags_Resizable | ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY;
    ImGuiContext& g = *ImGui::GetCurrentContext();

    static ImVec2 cellPadding(1.0f, 1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cellPadding);
    if (ImGui::BeginTable("Entries", 17, flags))
    {
        ImGuiTable* table = g.CurrentTable;

        static auto flags = ImGuiTableColumnFlags_WidthFixed;
        ImGui::TableSetupColumn("Title", flags, 500.0);
        ImGui::TableSetupColumn("Sorting Title", ImGuiTableColumnFlags_DefaultSort);
        ImGui::TableSetupColumn("Platform");
        ImGui::TableSetupColumn("Region", flags, 50.0);
        ImGui::TableSetupColumn("Release Year");
        ImGui::TableSetupColumn("Update Status", ImGuiTableColumnFlags_NoSort);
        ImGui::TableSetupColumn("Archived Version", ImGuiTableColumnFlags_NoSort);
        ImGui::TableSetupColumn("Best Version", ImGuiTableColumnFlags_NoSort);
        ImGui::TableSetupColumn("DLC", ImGuiTableColumnFlags_NoSort);
        ImGui::TableSetupColumn("Completion");
        ImGui::TableSetupColumn("Rating");
        ImGui::TableSetupColumn("S", flags | ImGuiTableColumnFlags_NoSort, 25.0);
        ImGui::TableSetupColumn("J", flags | ImGuiTableColumnFlags_NoSort, 25.0);
        ImGui::TableSetupColumn("T", flags | ImGuiTableColumnFlags_NoSort, 25.0);
        ImGui::TableSetupColumn("Last Played");
        ImGui::TableSetupColumn("N", flags | ImGuiTableColumnFlags_NoSort, 25.0);
        ImGui::TableSetupColumn("X", flags | ImGuiTableColumnFlags_NoSort, 25.0);

        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableHeadersRow();

        for (size_t i = 0; i < entryIdx; ++i)
        {
            if (ENTRIES[i].deleted)
                continue;

            if (search.IsActive())
            {
                if (!search.PassFilter(ENTRIES[i].title.c_str()))
                    continue;
            }

            // Filtering
            if (filter.active)
            {
                if (filter.platformActive)
                {
                    bool shouldSkip = true;
                    for (u64 j = 0; j < ARRAY_SZ(platformStr); ++j)
                    {
                        if (filter.platformsSelected[ENTRIES[i].platform])
                            shouldSkip = false;
                    }

                    if (shouldSkip)
                        continue;
                }

                if (filter.releaseYearActive)
                {
                    if (ENTRIES[i].releaseYear != filter.releaseYear)
                        continue;
                }

                if (filter.lastPlayedActive)
                {
                    if (ENTRIES[i].lastPlayed != filter.lastPlayed)
                        continue;
                }

                if (filter.ratingActive)
                {
                    if (ENTRIES[i].rating != filter.rating)
                        continue;
                }

                if (filter.completionActive)
                {
                    bool shouldSkip = true;
                    for (u64 j = 0; j < ARRAY_SZ(completionStr); ++j)
                    {
                        if (filter.completionsSelected[ENTRIES[i].completion])
                        {
                            shouldSkip = false;
                        }
                    }

                    if (shouldSkip)
                        continue;
                }

                // TODO These should probably be rephrased.
                if (filter.sActive)
                {
                    if (!ENTRIES[i].s) continue;
                }

                if (filter.jActive)
                {
                    if (!ENTRIES[i].j) continue;
                }

                if (filter.tActive)
                {
                    if (!ENTRIES[i].t) continue;
                }
            }

            ImGui::TableNextRow();

            if (focusNewEntry)
                ImGui::SetKeyboardFocusHere();

            ImGui::TableSetColumnIndex(0);
            ImGui::PushID(&ENTRIES[i].title);
            ImGui::PushItemWidth(-1);
            if (ImGui::InputTextWithHint("##On", "title", &ENTRIES[i].title))
                Excella::dirty = true;
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].sortingTitle);
            ImGui::PushItemWidth(-1);
            if (ImGui::InputTextWithHint("##On", "sorting title", &ENTRIES[i].sortingTitle))
                Excella::dirty = true;
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].platform);
            ImGui::PushItemWidth(-1);
            if (ImGui::BeginCombo("##On", platformStr[ENTRIES[i].platform]))
            {
                for (size_t n = 0; n < ARRAY_SZ(platformStr); ++n)
                {
                    const bool isSelected = (ENTRIES[i].platform == n);
                    if (ImGui::Selectable(platformStr[n], isSelected))
                    {
                        ENTRIES[i].platform = (Platform)n;
                        Excella::dirty = true;
                    }

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
                    {
                        ENTRIES[i].region = (Region)n;
                        Excella::dirty = true;
                    }

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
            if (ImGui::InputInt("##On", &ENTRIES[i].releaseYear))
                Excella::dirty = true;
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
                    {
                        ENTRIES[i].updateStatus = (ContentStatus)n;
                        Excella::dirty = true;
                    }

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
            if (ImGui::InputTextWithHint("##On", "archived version", &ENTRIES[i].archivedVersion))
                Excella::dirty = true;
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].bestVersion);
            ImGui::PushItemWidth(-1);
            if (ImGui::InputTextWithHint("##On", "known best version", &ENTRIES[i].bestVersion))
                Excella::dirty = true;
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].dlcStatus);
            ImGui::PushItemWidth(-1);
            if (ImGui::BeginCombo("##On", contentStatusStr[ENTRIES[i].dlcStatus]))
            {
                for (size_t n = 0; n < 5; ++n)
                {
                    const bool isSelected = (ENTRIES[i].dlcStatus == n);
                    if (ImGui::Selectable(contentStatusStr[n], isSelected))
                    {
                        ENTRIES[i].dlcStatus = (ContentStatus)n;
                        Excella::dirty = true;
                    }

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
                    {
                        ENTRIES[i].completion = (Completion)n;
                        Excella::dirty = true;
                    }

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
            if (ImGui::InputInt("##On", &ENTRIES[i].rating))
                Excella::dirty = true;
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].s);
            ImGui::PushItemWidth(-1);
            if (ImGui::Checkbox("##On", &ENTRIES[i].s))
                Excella::dirty = true;
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].j);
            ImGui::PushItemWidth(-1);
            if (ImGui::Checkbox("##On", &ENTRIES[i].j))
                Excella::dirty = true;
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].t);
            ImGui::PushItemWidth(-1);
            if (ImGui::Checkbox("##On", &ENTRIES[i].t))
                Excella::dirty = true;
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].lastPlayed);
            ImGui::PushItemWidth(-1);
            if (ImGui::InputInt("##On", &ENTRIES[i].lastPlayed))
                Excella::dirty = true;
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::TableNextColumn();
            ImGui::PushID(&ENTRIES[i].notes);
            ImGui::PushItemWidth(-1);
            static Texture edit = load_texture_from_memory(&editBytes, editBytesSize);
            static Texture editWhite =
                load_texture_from_memory(&editWhiteBytes, editWhiteBytesSize);
            Texture* useEdit = ENTRIES[i].notes.size() == 0 ? &edit : &editWhite;
            if (ImGui::ImageButton("", (void*)(intptr_t)useEdit->data, ImVec2(18, 18)))
            {
                ImGui::OpenPopup("Edit Notes");
            }
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            if (ImGui::BeginPopupModal("Edit Notes", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                if (ImGui::InputTextMultiline("##On", &ENTRIES[i].notes, ImVec2(500, 500), flags))
                    Excella::dirty = true;
                if (ImGui::Button("Close"))
                    ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }
            (void)ImGui::PopItemWidth();
            ImGui::PopID();

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

            if (focusNewEntry)
                ImGui::SetScrollHereY(0.5f);

            // Highlight entries based on archive status
            static auto orange = ImVec4(0.6f, 0.4f, 0.2f, 1.0f);
            static auto red = ImVec4(0.6f, 0.2f, 0.2f, 1.0f);
            static auto yellow = ImVec4(0.6f, 0.6f, 0.0f, 1.0f);
            static auto green = ImVec4(0.2f, 0.6f, 0.4f, 1.0f);

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

            if ((e.dlcStatus == DOWNLOADED || e.dlcStatus == CS_NONE) &&
                (e.updateStatus == DOWNLOADED || e.updateStatus == LOCKED) &&
                e.completion == COMPLETED)
                table->RowBgColor[1] = ImGui::GetColorU32(green);

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
            // !ImGui::IsAnyItemHovered(); This can optionally disable row highlighting if any cells
            // are hovered.

            if (hover)
                table->RowBgColor[1] =
                    ImGui::GetColorU32(ImGuiCol_Border); // set to any color of your choice
        }

        // Sorting table entries
        // clang-format off
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
                                sort = numeric < 0 ? true : false;
                            }; break;

                            // Sorting Title
                            case 1: {
                                auto numeric = lhs.sortingTitle.compare(rhs.sortingTitle);
                                sort = numeric < 0 ? true : false;
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
                                if (currentSpecs->SortDirection == ImGuiSortDirection_Ascending)
                                {
                                    return lhs.rating < rhs.rating;
                                }
                                else
                                {
                                    if (lhs.rating > rhs.rating || rhs.rating < lhs.rating)
                                        return true;

                                    return false;
                                }
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
                sortSpecs->SpecsDirty = false;
                sortSpecs->SpecsDirty = false;
            }
        }
        // clang-format on
        ImGui::EndTable();
    }
    ImGui::PopStyleVar();
}

void update_imgui(GLFWwindow* window)
{
    bool focusSearch = false;
    bool focusNewEntry = false;

    static auto browserFlags = ImGuiFileBrowserFlags_EnterNewFilename |
                               ImGuiFileBrowserFlags_CloseOnEsc |
                               ImGuiFileBrowserFlags_CreateNewDir;
    static ImGui::FileBrowser browser(browserFlags);
    static bool browserWantsSave = false;
    static bool browserWantsLoad = false;

    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_Q))
    {
        // TODO (Mads): Show save prompt if excella dirty
        glfwSetWindowShouldClose(window, true);
    }

    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_S))
        save_database(Excella::activeDbPath.c_str());

    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_N))
    {
        search.Clear();
        filter = {};
        create_entry();
        focusNewEntry = true;
    }

    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_F))
        focusSearch = true;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    static auto flags =
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    ImGui::Begin("Excella", nullptr, flags);
    {
        static void (*showPopup)() = []() {};

        // main menu
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

                if (ImGui::MenuItem("Save", "CTRL+s"))
                {
                    save_database(Excella::activeDbPath.c_str());
                }

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
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Quit", "CTRL+q"))
                {
                    if (Excella::dirty)
                    {
                        showPopup = []() {
                            if (!ImGui::IsPopupOpen("Unsaved Changes"))
                                ImGui::OpenPopup("Unsaved Changes");

                            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
                            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing,
                                                    ImVec2(0.5f, 0.5f));
                            if (ImGui::BeginPopupModal("Unsaved Changes"))
                            {
                                ImGui::Text("Save Changes to document before closing?");
                                ImGui::Text("Your changes will be lost if you don't save them.");

                                if (ImGui::Button("Don't Save", ImVec2(80, 0)))
                                {
                                    ImGui::CloseCurrentPopup();
                                    showPopup = []() {};
                                }

                                ImGui::SameLine();
                                if (ImGui::Button("Cancel", ImVec2(80, 0)))
                                {
                                    ImGui::CloseCurrentPopup();
                                    showPopup = []() {};
                                }

                                ImGui::SameLine();
                                if (ImGui::Button("Save", ImVec2(80, 0)))
                                {
                                    save_database(Excella::activeDbPath.c_str());
                                    ImGui::CloseCurrentPopup();
                                    showPopup = []() {};
                                }
                                ImGui::EndPopup();
                            }
                        };
                    }
                    glfwSetWindowShouldClose(window, true);
                }

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

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Misc"))
            {
                if (ImGui::MenuItem("Statistics"))
                {
                    // TODO: This needs a complete re-write. It might even be merged with the filter.
                    showPopup = []() {
                        if (!ImGui::IsPopupOpen("Statistics"))
                            ImGui::OpenPopup("Statistics");

                        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
                        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
                        if (ImGui::BeginPopupModal("Statistics"))
                        {
                            ImGui::Text("Current entries: %i", Excella::actualTotalEntries);
                            ImGui::Text("Max Supported entries: %zu", ENTRIES_MAX);

                            i32 countBeaten = 0;
                            i32 countCompleted = 0;
                            i32 countEndless = 0;
                            i32 countRetired = 0;
                            for (size_t i = 0; i < entryIdx; ++i)
                            {
                                // clang-format off
                                switch (ENTRIES[i].completion) {
                                case BEATEN:    ++countBeaten;    break;
                                case COMPLETED: ++countCompleted; break;
                                case ENDLESS:   ++countEndless;   break;
                                case RETIRED:    ++countRetired;  break;
                                default: continue;
                                }
                                // clang-format on
                            }

                            ImGui::Separator();
                            ImGui::Text("Play Status");
                            ImGui::Text("\tBeaten: %i", countBeaten);
                            ImGui::Text("\tCompleted: %i", countCompleted);
                            ImGui::Text("\tEndless: %i", countEndless);
                            ImGui::Text("\tRetired: %i", countRetired);

                            ImGui::Separator();
                            ImGui::Text("Platform Breakdown");
                            for (size_t i = 0; i < ARRAY_SZ(platformStr); ++i)
                            {
                                i32 count = 0;
                                for (size_t j = 0; j < entryIdx; ++j)
                                {
                                    if (ENTRIES[j].platform == i)
                                        ++count;
                                }
                                ImGui::Text("\t%s: %i", platformStr[i], count);
                            }

                            if (ImGui::Button("Close", ImVec2(80, 0)))
                            {
                                ImGui::CloseCurrentPopup();
                                showPopup = []() {};
                            }
                            ImGui::EndPopup();
                        }
                    };
                }

                ImGui::EndMenu();
            }

            ImGui::Text("%s", Excella::activeDbPath.c_str());

            static Texture disketteRed = load_texture_from_memory(&disketteRedBytes, disketteRedBytesSize);
            static Texture disketteGray = load_texture_from_memory(&disketteGrayBytes, disketteGrayBytesSize);
            Texture& diskette = Excella::dirty ? disketteRed : disketteGray;
            if (ImGui::ImageButton("", (void*)(intptr_t)diskette.data, ImVec2(18, 18)))
            {
                save_database(Excella::activeDbPath.c_str());
            }

            ImGui::EndMainMenuBar();
        }

        browser.Display();

        if (browser.HasSelected() && browserWantsSave)
        {
            std::string newPath = browser.GetSelected().string();
            Excella::activeDbPath = newPath.c_str();
            save_database(Excella::activeDbPath.c_str());
            browser.ClearSelected();
            reset_database();
            load_database(Excella::activeDbPath.c_str());
        }

        if (browser.HasSelected() && browserWantsLoad)
        {
            std::string newPath = browser.GetSelected().string();
            Excella::activeDbPath = newPath.c_str();
            reset_database();
            load_database(Excella::activeDbPath.c_str());
            browser.ClearSelected();
        }

        showPopup();
        draw_table(focusSearch, focusNewEntry);
    }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
