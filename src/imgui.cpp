#include "imgui.hpp"
#include "core.hpp"
#include "database.hpp"
#include "entry.hpp"
#include "texture.hpp"

#include <GLFW/glfw3.h>

#include <algorithm>

void draw_main_menu(GLFWwindow* window, const char* fp)
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
                save_database_to_file(fp);
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

void draw_table(bool focusFilter, bool focusNewEntry)
{
    if (focusFilter)
        ImGui::SetKeyboardFocusHere();
    ImGui::SeparatorText("Filter");
    ImGui::PushItemWidth(-1);
    static ImGuiTextFilter filter;
    (void)ImGui::PopItemWidth;
    filter.Draw();

    ImGui::Separator();
    static auto flags =
        ImGuiTableFlags_Resizable | ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY;
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

        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableHeadersRow();

        for (size_t i = 0; i < entryIdx; ++i)
        {
            if (ENTRIES[i].deleted)
                continue;

            if (filter.IsActive())
            {
                if (!filter.PassFilter(ENTRIES[i].title.c_str()))
                    continue;
            }

            ImGui::TableNextRow();

            if (focusNewEntry)
                ImGui::SetKeyboardFocusHere();

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

            if (focusNewEntry)
                ImGui::SetScrollHereY(0.5f);

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
