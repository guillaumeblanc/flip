#include "flip/utils/loader.h"

#include <cassert>
#include <type_traits>
#include <vector>

#include "stb/stb_image.h"

using namespace std::placeholders;

namespace flip {

namespace {

struct UserData {
  std::vector<std::byte>* buffer = nullptr;
  AsyncBuffer::Completion* completion = nullptr;
};
static_assert(std::is_trivially_copyable<UserData>{});
}  // namespace

AsyncBuffer::AsyncBuffer(const char* _filename, const Completion& _completion,
                         size_t _buffering_size) {
  auto user_data =
      UserData{.buffer = new std::vector<std::byte>{},
               .completion = new AsyncBuffer::Completion{_completion}};

  auto stream_buffer = malloc(_buffering_size);
  handle_ =
      sfetch_send(sfetch_request_t{.path = _filename,
                                   .callback = &FetchCallback,
                                   .chunk_size = _buffering_size,
                                   .buffer = {stream_buffer, _buffering_size},
                                   .user_data = SFETCH_RANGE(user_data)});
  assert(sfetch_handle_valid(handle_));
}

AsyncBuffer::~AsyncBuffer() { sfetch_cancel(handle_); }

void AsyncBuffer::FetchCallback(const sfetch_response_t* _reponse) {
  auto& user_data = *reinterpret_cast<UserData*>(_reponse->user_data);

  if (_reponse->fetched && !_reponse->cancelled) {
    auto& buffer = *user_data.buffer;

    // Append fetched data
    const auto min_size = _reponse->data_offset + _reponse->data.size;
    if (min_size > buffer.size()) {
      buffer.resize(min_size);
    }
    const std::byte* stream = static_cast<const std::byte*>(_reponse->data.ptr);
    std::copy(stream, stream + _reponse->data.size,
              buffer.begin() + _reponse->data_offset);
  }

  if (_reponse->finished) {
    if (_reponse->cancelled) {
      // Cancellation must be checked first, because it means AsyncBuffer is
      // delete and cannot be accessed.
    } else {
      (*user_data.completion)(!_reponse->failed, *user_data.buffer,
                              _reponse->path);
    }

    // Free streaming buffer
    auto* scratch = sfetch_unbind_buffer(_reponse->handle);
    free(scratch);

    // Free user data
    delete user_data.buffer;
    delete user_data.completion;
  }
}

namespace {
void LoadImageImpl(std::span<const std::byte> _buffer, sg_image _image,
                   const char* _name) {
  int width, height, channels;
  stbi_uc* pixels = stbi_load_from_memory(
      reinterpret_cast<const stbi_uc*>(_buffer.data()),
      static_cast<int>(_buffer.size_bytes()), &width, &height, &channels, 4);
  if (!pixels) {
    // Failed to load image from buffer
    const char* error = stbi_failure_reason();
    sg_fail_image(_image);
  } else {
    // Init image from pixels
    sg_init_image(
        _image,
        sg_image_desc{.width = width,
                      .height = height,
                      .pixel_format = SG_PIXELFORMAT_RGBA8,
                      .data = {.subimage = {{{.ptr = pixels,
                                              .size = static_cast<size_t>(
                                                  width * height * 4)}}}},
                      .label = _name});
    stbi_image_free(pixels);
  }
}
}  // namespace

SgAsyncImage::SgAsyncImage(const char* _filename)
    : image_{sg_alloc_image()},
      buffer_{_filename,
              std::bind(&SgAsyncImage::Completed, image_.id(), _1, _2, _3)} {}

void SgAsyncImage::Completed(sg_image _image, bool _successs,
                             std::span<const std::byte> _buffer,
                             const char* _filename) {
  assert(sg_query_image_state(_image) == SG_RESOURCESTATE_ALLOC);
  if (_successs) {
    LoadImageImpl(_buffer, _image, _filename);
  } else {
    sg_fail_image(_image);
  }
}

}  // namespace flip