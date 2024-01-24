#pragma once

#include "core/integer.hpp"

void init_glfw(i32 width, i32 height);
bool update_glfw_events();	// returns true if glfw window should close
void clear_render_context();
void swap_buffers();