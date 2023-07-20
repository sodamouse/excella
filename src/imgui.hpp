#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_stdlib.h>

void draw_main_menu(GLFWwindow* window, const char* fp);
void draw_table(bool focusFilter, bool focusNewEntry);
void update_imgui(GLFWwindow* winodw);
