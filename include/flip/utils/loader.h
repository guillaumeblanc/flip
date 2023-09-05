#pragma once

#include <span>
#include <vector>

#include "flip/utils/sokol_gfx.h"

namespace flip {

std::vector<std::byte> Load(const char* _filename);

SgImage LoadImage(std::span<const std::byte> _buffer);
}  // namespace flip