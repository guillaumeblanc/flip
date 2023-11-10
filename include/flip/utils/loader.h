#pragma once

#include <functional>
#include <span>

#include "flip/utils/sokol_gfx.h"
#include "sokol/sokol_fetch.h"

namespace flip {

// Create an asynchronous buffer reading from file.
class AsyncBuffer {
 public:
  AsyncBuffer() = default;

  // Callback function notifiying loading success or failure
  using Completion = std::function<void(bool, std::span<const std::byte>,
                                        const char* _filename)>;

  // Initiate a loading request
  AsyncBuffer(const char* _filename, const Completion& _completion,
              size_t _buffering_size = 4 << 10);

  // Immediately cancels async operation on destruction.
  ~AsyncBuffer();

  // Movable
  AsyncBuffer(AsyncBuffer&& _sa) { std::swap(handle_, _sa.handle_); }
  AsyncBuffer& operator=(AsyncBuffer&& _sa) {
    if (this != std::addressof(_sa)) {
      std::swap(handle_, _sa.handle_);
    }
    return *this;
  }

  // Not copyable
  AsyncBuffer(const AsyncBuffer&) = delete;
  AsyncBuffer& operator=(const AsyncBuffer&) = delete;

 private:
  // Important to use a static function, as *this is movable.
  static void FetchCallback(const sfetch_response_t* _reponse);

  sfetch_handle_t handle_ = {};
};

// Create image read asynchronously from a file.
class SgAsyncImage {
 public:
  SgAsyncImage() = default;
  explicit SgAsyncImage(const char* _filename);

  // Immediately cancels async operation and destroy image.
  ~SgAsyncImage() = default;

  // Movable
  SgAsyncImage(SgAsyncImage&&) = default;
  SgAsyncImage& operator=(SgAsyncImage&& _sai) = default;

  // Not copyable
  SgAsyncImage(const SgAsyncImage&) = delete;
  SgAsyncImage& operator=(const SgAsyncImage&) = delete;

  bool is_valid() const { return image_.is_valid(); }

  sg_image id() const { return image_.id(); }

 private:
  // Important to use a static function, as *this is movable.
  // This function will only be called if buffer_ isn't deleted / cancelled.
  // Hence it's safe to pass it image_;
  static void Completed(sg_image _image, bool _successs,
                        std::span<const std::byte> _buffer,
                        const char* _filename);

  SgImage image_;
  AsyncBuffer buffer_;
};

}  // namespace flip
