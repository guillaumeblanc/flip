#include "flip/utils/loader.h"

#include <fstream>

#include "stb/stb_image.h"

namespace flip {
std::vector<std::byte> Load(const char* _filename) {
  auto content = std::vector<std::byte>();

  auto file = std::ifstream(_filename, std::ios::binary);
  if (file.is_open()) {
    // File size
    file.seekg(0, file.end);
    auto length = file.tellg();
    file.seekg(0, file.beg);

    // Reads all
    content.resize(length);
    file.read(reinterpret_cast<char*>(content.data()), content.size());
  }

  return content;
}

SgImage LoadImage(std::span<const std::byte> _buffer) {
  auto image = flip::SgImage{sg_alloc_image()};

  int width, height, channels;
  stbi_uc* pixels = stbi_load_from_memory(
      reinterpret_cast<const stbi_uc*>(_buffer.data()),
      static_cast<int>(_buffer.size_bytes()), &width, &height, &channels, 4);
  if (!pixels) {
    // Failed to load image from buffer
    const char* error = stbi_failure_reason();
  } else {
    // Init image from pixels
    sg_init_image(image,
                  sg_image_desc{
                      .width = width,
                      .height = height,
                      .pixel_format = SG_PIXELFORMAT_RGBA8,
                      .data = {.subimage = {{{.ptr = pixels,
                                              .size = static_cast<size_t>(
                                                  width * height * 4)}}}},
                      .label = "Texture sample",
                  });
    stbi_image_free(pixels);
  }

  // Returns image anyway, uses image state to know it's status
  return image;
}
}  // namespace flip