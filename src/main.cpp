// COPYRIGHT (C) sodamouse - See LICENSE.md

#include "excella.hpp"
#include "comfyg.hpp"
#include "core.hpp"
#include "database.hpp"
#include "entry.hpp"
#include "font.hpp"
#include "imgui.hpp"

#include <GLFW/glfw3.h>

#include <thread>

int main()
{
    std::string username = std::getenv("USER");
    std::string configFilePath = "/home/" + username + "/.config/excella/excella.conf";
    const char** dbPath = Comfyg::config_str("database_path", "excella.db");
    Comfyg::load_config_file(configFilePath.c_str());

    std::thread entryLoader(load_database, *dbPath);

    if (!glfwInit())
        return 1;

    GLFWwindow* window = glfwCreateWindow(800, 600, Excella::version, nullptr, nullptr);
    if (!window)
        return 1;

    glfwMakeContextCurrent(window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromMemoryTTF(fontBytes, sizeof(fontBytes), 18);
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.0, 0.2, 0.4, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        update_imgui(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    entryLoader.join();

    return 0;
}
