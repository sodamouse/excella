#include "window.hpp"
#include "excella.hpp"
#include "core/integer.hpp"

#include <GLFW/glfw3.h>

#include <iostream>

void init_glfw(i32 width, i32 height)
{
    glfwSetErrorCallback([](int code, const char* msg) {
        std::cout << "[GLFW] " << code << ": " << msg << '\n';
        exit(code);
    });

    glfwInit();

    Excella::window = glfwCreateWindow(width, height, Excella::version, nullptr, nullptr);

    glfwMakeContextCurrent(Excella::window);
    glfwMaximizeWindow(Excella::window);
}

bool update_glfw_events()
{
    glfwPollEvents();
    return glfwWindowShouldClose(Excella::window);
}

void clear_render_context()
{
    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
}

void swap_buffers()
{
    glfwSwapBuffers(Excella::window);
}