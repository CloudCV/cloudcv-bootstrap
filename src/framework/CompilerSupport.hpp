#pragma once

#include <stdexcept>

#if _MSC_VER

#define CLOUDCV_NOTHROW _THROW0()

#else

#define CLOUDCV_NOTHROW noexcept

#endif