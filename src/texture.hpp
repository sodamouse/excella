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

constexpr size_t editWhiteBytesSize = 1380;
extern byte editWhiteBytes[1380];

constexpr size_t disketteRedBytesSize = 1484;
extern byte disketteRedBytes[1484];

constexpr size_t disketteGrayBytesSize = 9844;
extern byte disketteGrayBytes[9844];
