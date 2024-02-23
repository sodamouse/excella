#include "comfyg.hpp"
#include "window.hpp"
#include "database.hpp"
#include "entry.hpp"
#include "excella.hpp"
#include "imgui.hpp"

#include <filesystem>
#include <fstream>

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

    using namespace std::chrono_literals;
    while (!Excella::quit)
    {
        update_glfw_events();
        // @TODO: Window re-size events
        clear_render_context();
        update();
        swap_buffers();

        std::this_thread::sleep_for(10ms);
    }

    entryLoader.join();

    return 0;
}
