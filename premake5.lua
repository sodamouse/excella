workspace "Amelie"
    location "./build"
    configurations { "debug", "release" }

project "imgui"
    kind "StaticLib"
    language "c++"
    toolset "clang"
    files { "src/imgui/*" }

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

project "amelie"
    kind "ConsoleApp"
    language "c++"
    toolset "clang"
    files { "src/*" }

    buildoptions {
        "--std=c++20",
        "-Wall",
        "-Wextra",
        "-Wpedantic",
    }

    links {
        "GL",
        "glfw",
        "imgui",
    }

    -- "{COPY} ../res/* %{cfg.targetdir}"
    postbuildcommands {
        "{COPY} ../res/* ./bin/debug"
    }

    filter "configurations:debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:release"
        defines { "NDEBUG" }
        optimize "On"
