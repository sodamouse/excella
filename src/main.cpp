// COPYRIGHT (C) sodamouse - See LICENSE.md

#include "comfyg.hpp"
#include "core.hpp"
#include "database.hpp"
#include "entry.hpp"
#include "imgui.hpp"

#include <GLFW/glfw3.h>

#include <thread>

namespace Amelie {
const char* version = "v1.5.0";
const char* activeDbPath;
} // namespace Amelie

int main()
{
    std::string username = std::getenv("USER");
    std::string configFilePath = "/home/" + username + "/.config/amelie/amelie.conf";
    const char** dbPath = Comfyg::config_str("database_path", "amelie.db");
    Comfyg::load_config_file(configFilePath.c_str());

    std::thread entryLoader(load_database, *dbPath, &Amelie::activeDbPath);

    if (!glfwInit())
        return 1;

    GLFWwindow* window = glfwCreateWindow(800, 600, "Amelie", nullptr, nullptr);
    if (!window)
        return 1;

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
        bool focusFilter = false;
        bool focusNewEntry = false;

        if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_Q))
            glfwSetWindowShouldClose(window, true);

        if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_S))
            save_database_to_file(Amelie::activeDbPath);

        if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_N))
        {
            create_entry();
            focusNewEntry = true;
        }

        if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_F))
            focusFilter = true;

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
            draw_main_menu(window, Amelie::activeDbPath);
            draw_table(focusFilter, focusNewEntry);
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    entryLoader.join();

    return 0;
}
