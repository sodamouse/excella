workspace "Excella"
    location "./build"
    configurations { "debug", "release" }

project "imgui"
    kind "StaticLib"
    language "c++"
    toolset "clang"
    files { "vendor/imgui/*" }

    buildoptions {
        "--std=c++20",
        "-Wall",
        "-Wextra",
        "-Wpedantic",
    }

    filter "configurations:debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:release"
        defines { "NDEBUG" }
        optimize "On"

project "excella"
    kind "ConsoleApp"
    language "c++"
    toolset "clang"
    files {
        "src/**",
    }

    buildoptions {
        "--std=c++20",
        "-Wall",
        "-Wextra",
        "-Wpedantic",
    }

    includedirs {
        "vendor"
    }

    -- TODO (Mads) glfw should be a vendor include
    -- TODO (Mads) on windows, opengl library is called differently
    links {
        "glfw",
        "imgui",
    }

    filter "system:linux"
        defines { "OS_UNIX" }
        links { "GL" }

    filter "system:windows"
        defines { "OS_WINDOWS" }

    filter "configurations:debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:release"
        defines { "NDEBUG" }
        optimize "On"
