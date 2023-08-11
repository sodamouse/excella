#pragma once

#ifdef PROFILE
#include <tracy/public/tracy/Tracy.hpp>
#define MARK FrameMark
#else
#define MARK
#endif
