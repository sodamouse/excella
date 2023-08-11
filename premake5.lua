workspace "Excella"
    location "./build"
    configurations { "debug", "release", "profile" }

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
        "vendor/tracy/public/TracyClient.cpp"
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

    links {
        "GL",
        "glfw",
        "imgui",
    }

    filter "configurations:debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:release"
        defines { "NDEBUG" }
        optimize "On"

    filter "configurations:profile"
        defines { "PROFILE", "TRACY_ENABLE" }
        symbols "On"
