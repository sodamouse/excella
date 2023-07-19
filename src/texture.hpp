#pragma once

#include "integer.hpp"
#include <GLFW/glfw3.h>

struct Texture
{
    GLuint data;
    i32 width;
    i32 height;
};

Texture load_texture_from_file(const char* filename);
