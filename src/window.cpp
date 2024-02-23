#include "window.hpp"
#include "excella.hpp"
#include "core/integer.hpp"
#include "texture.hpp"

#include <GLFW/glfw3.h>
#include <stb_image/stb_image.h>

#include <iostream>

void init_glfw(i32 width, i32 height)
{
    glfwSetErrorCallback([](int code, const char* msg) {
        std::cout << "[GLFW] " << code << ": " << msg << '\n';
        exit(code);
    });

    glfwInit();

    Excella::window = glfwCreateWindow(width, height, Excella::version, nullptr, nullptr);

    GLFWimage icons[1];
    icons[0].pixels = stbi_load_from_memory(iconBytes, iconBytesSize, &icons[0].width, &icons[0].height, 0, 4);
    glfwSetWindowIcon(Excella::window, 1, icons);

    glfwMakeContextCurrent(Excella::window);
    glfwMaximizeWindow(Excella::window);
}

void update_glfw_events()
{
    glfwPollEvents();
    if(glfwWindowShouldClose(Excella::window)) Excella::quit = true;
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
