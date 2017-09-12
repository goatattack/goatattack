#ifndef _FT_HPP_
#define _FT_HPP_

#ifdef __APPLE__
#include "ft2build.h"
#else
#include <ft2build.h>
#endif

#ifdef __clang__
#pragma clang diagnostic ignored "-Wc++11-long-long"
#endif

#include FT_FREETYPE_H
#include FT_STROKER_H
#include FT_BITMAP_H

#endif
