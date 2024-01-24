// COPYRIGHT (C) sodamouse - See LICENSE.md

#include "comfyg.hpp"
#include "core/core.hpp"
#include "database.hpp"
#include "entry.hpp"
#include "excella.hpp"
#include "font.hpp"
#include "imgui.hpp"

#include <GLFW/glfw3.h>

#include <filesystem>
#include <fstream>
#include <thread>
#include <chrono>

#ifdef OS_WINDOWS
#include <Windows.h>
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
#else
int main()
#endif
{
    const char** dbPath = Comfyg::config_str("database_path", "./excella.db");

#ifdef OS_UNIX
    std::string username       = std::getenv("USER");
    std::string configFilePath = "/home/" + username + "/.config/excella/excella.conf";
    const char** cacheFilePath = Comfyg::config_str("cache_file_path", "./cache.db");
#endif // OS_UNIX

#ifdef OS_WINDOWS
    std::string username       = std::getenv("USERNAME");
    std::string configFilePath = "C:/Users/" + username + "/Documents/excella/excella.conf";
    Excella::cacheFilePath     = "C:/Users/" + username + "/Documents/excella/excella.cache";
#endif // OS_WINDOWS

    Comfyg::load_config_file(configFilePath.c_str());

    if (std::filesystem::exists(Excella::cacheFilePath))
    {
        std::fstream cache(Excella::cacheFilePath, std::ios::in);
        std::string line;
        while (cache >> line)
        {
            Excella::cachedDbPaths.push_back(line);
        }
    }

    std::thread entryLoader(load_database, *dbPath);

    if (!glfwInit()) return 1;

    Excella::window = glfwCreateWindow(800, 600, Excella::version, nullptr, nullptr);
    if (!Excella::window) return 1;

    glfwMakeContextCurrent(Excella::window);
    glfwMaximizeWindow(Excella::window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromMemoryTTF(fontBytes, sizeof(fontBytes), 18);
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui_ImplGlfw_InitForOpenGL(Excella::window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    constexpr double FPS = 1000 / 60.0;
    auto lastTime = std::chrono::steady_clock::now();

    while (!glfwWindowShouldClose(Excella::window))
    {
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double, std::milli> deltaTime = now - lastTime;
        lastTime = now;

        std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(FPS - deltaTime.count()));

        glClearColor(0.1, 0.1, 0.1, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        update();

        glfwPollEvents();
        glfwSwapBuffers(Excella::window);
    }

    entryLoader.join();

    return 0;
}
