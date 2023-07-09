# ------------------------------------------------------------------------------
# COPYRIGHT (C) 2023 - sodamouse - See LICENCE.md
# ------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.20)
project(amelie)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ------------------------------------------------------------------------------
# Vendor
# ------------------------------------------------------------------------------
set(IMGUI_SOURCE_FILES
  src/imgui/imconfig.h
  src/imgui/imgui.cpp
  src/imgui/imgui.h
  src/imgui/imgui_demo.cpp
  src/imgui/imgui_draw.cpp
  src/imgui/imgui_impl_glfw.cpp
  src/imgui/imgui_impl_glfw.h
  src/imgui/imgui_impl_opengl3.cpp
  src/imgui/imgui_impl_opengl3.h
  src/imgui/imgui_impl_opengl3_loader.h
  src/imgui/imgui_internal.h
  src/imgui/imgui_stdlib.cpp
  src/imgui/imgui_stdlib.h
  src/imgui/imgui_tables.cpp
  src/imgui/imgui_widgets.cpp
  src/imgui/imstb_rectpack.h
  src/imgui/imstb_textedit.h
  src/imgui/imstb_truetype.h
)

find_package(glfw3 REQUIRED)

# ------------------------------------------------------------------------------
# Source files
# ------------------------------------------------------------------------------
set(AMELIE_SOURCE_FILES
  src/comfyg.cpp
  src/comfyg.hpp
  src/json.hpp
  src/main.cpp
  src/miriam.cpp
  src/miriam.hpp
  src/stb_image.cpp
  src/stb_image.h
)

# ------------------------------------------------------------------------------
# Targets
# ------------------------------------------------------------------------------
add_library(imgui ${IMGUI_SOURCE_FILES})
add_executable(amelie ${AMELIE_SOURCE_FILES})
target_compile_options(amelie PRIVATE -Werror -Wall -Wextra -Wpedantic)
target_link_libraries(amelie PRIVATE glfw GL imgui)
