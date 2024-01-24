// COPYRIGHT (C) sodamouse - See LICENSE.md

#include "comfyg.hpp"
#include "window.hpp"
#include "database.hpp"
#include "entry.hpp"
#include "excella.hpp"
#include "imgui.hpp"

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
        while (cache >> line) Excella::cachedDbPaths.push_back(line);
    }

    std::thread entryLoader(load_database, *dbPath);

    init_glfw(800, 600);

    init_imgui();

    constexpr double FPS = 1000 / 60.0;
    auto lastTime = std::chrono::steady_clock::now();

    bool quit = false;
    while (!quit)
    {
        if (update_glfw_events()) quit = true;

        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double, std::milli> deltaTime = now - lastTime;
        lastTime = now;
        std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(FPS - deltaTime.count()));

        clear_render_context();
        update();
        swap_buffers();
    }

    entryLoader.join();

    return 0;
}
