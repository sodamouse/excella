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
Texture load_texture_from_memory(void* data, u64 size);

constexpr u64 trashcanBytesSize = 14490;
extern byte trashcanBytes[14490];

constexpr u64 editBytesSize = 706;
extern byte editBytes[706];

constexpr u64 editWhiteBytesSize = 1380;
extern byte editWhiteBytes[1380];

constexpr u64 disketteRedBytesSize = 1484;
extern byte disketteRedBytes[1484];

constexpr u64 disketteGrayBytesSize = 9844;
extern byte disketteGrayBytes[9844];

constexpr u64 tagGreenBytesSize = 1995;
extern byte tagGreenBytes[1995];

constexpr u64 tagBlackBytesSize = 1575;
extern byte tagBlackBytes[1575];