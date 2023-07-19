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
Texture load_texture_from_memory(void* data, size_t size);

constexpr size_t trashcanBytesSize = 14490;
extern byte trashcanBytes[14490];

constexpr size_t editBytesSize = 706;
extern byte editBytes[706];
