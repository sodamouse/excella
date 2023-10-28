workspace "Excella"
    location "./build"
    configurations { "debug", "release" }

project "glfw"
    kind "StaticLib"
    language "C"
    files {
        "vendor/glfw/src/cocoa_time.c",
        "vendor/glfw/src/context.c",
        "vendor/glfw/src/egl_context.c",
        "vendor/glfw/src/glx_context.c",
        "vendor/glfw/src/init.c",
        "vendor/glfw/src/input.c",
        "vendor/glfw/src/linux_joystick.c",
        "vendor/glfw/src/monitor.c",
        "vendor/glfw/src/null_init.c",
        "vendor/glfw/src/null_joystick.c",
        "vendor/glfw/src/null_monitor.c",
        "vendor/glfw/src/null_window.c",
        "vendor/glfw/src/osmesa_context.c",
        "vendor/glfw/src/platform.c",
        "vendor/glfw/src/posix_module.c",
        "vendor/glfw/src/posix_poll.c",
        "vendor/glfw/src/posix_thread.c",
        "vendor/glfw/src/posix_time.c",
        "vendor/glfw/src/vulkan.c",
        "vendor/glfw/src/wgl_context.c",
        "vendor/glfw/src/win32_init.c",
        "vendor/glfw/src/win32_joystick.c",
        "vendor/glfw/src/win32_module.c",
        "vendor/glfw/src/win32_monitor.c",
        "vendor/glfw/src/win32_thread.c",
        "vendor/glfw/src/win32_time.c",
        "vendor/glfw/src/win32_window.c",
        "vendor/glfw/src/window.c",
        "vendor/glfw/src/wl_init.c",
        "vendor/glfw/src/wl_monitor.c",
        "vendor/glfw/src/wl_window.c",
        "vendor/glfw/src/x11_init.c",
        "vendor/glfw/src/x11_monitor.c",
        "vendor/glfw/src/x11_window.c",
        "vendor/glfw/src/xkb_unicode.c"
    }

    architecture "x86_64"

    filter "system:linux"
        defines { "_GLFW_X11" }

    filter "system:windows"
        defines { "_GLFW_WIN32" }

    filter "configurations:debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:release"
        defines { "NDEBUG" }
        optimize "On"

project "imgui"
    kind "StaticLib"
    language "c++"
    files { "vendor/imgui/*" }

    buildoptions {
        "--std=c++20",
    }

    links {
        "glfw"
    }

    includedirs {
        "vendor/glfw/include"
    }

    architecture "x86_64"

    filter "configurations:debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:release"
        defines { "NDEBUG" }
        optimize "On"

project "excella"
    kind "ConsoleApp"
    language "c++"
    files {
        "src/**",
    }

    cppdialect "C++20"

    includedirs {
        "vendor",
        "vendor/glfw/include"
    }

    links {
        "glfw",
        "imgui",
    }

    architecture "x86_64"

    filter "system:linux"
        defines { "OS_UNIX" }
        links { "GL" }

    filter "system:windows"
        defines { "OS_WINDOWS" }
        links { "opengl32" }

    filter "configurations:debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:release"
        defines { "NDEBUG" }
        optimize "On"
