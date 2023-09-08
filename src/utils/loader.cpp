#include "flip/utils/loader.h"

#include <cassert>
#include <fstream>

#include "sokol/sokol_fetch.h"
#include "stb/stb_image.h"

namespace flip {

namespace {

// Create an synchronous buffer reading from file
class AsyncBuffer {
 public:
  // Buffer
  using Buffer = std::vector<std::byte>;

  AsyncBuffer(const char* _filename) {
    // Scratch buffer for streaming, will freed on request finish.
    const size_t kStreamingSize = 256;
    auto buffer = malloc(kStreamingSize);

    // Prepares and send request
    handle_ = sfetch_send(sfetch_request_t{.path = _filename,
                                           .callback = &FetchCallback,
                                           .chunk_size = kStreamingSize,
                                           .buffer = {buffer, kStreamingSize},
                                           .user_data = to_user_data()});
    assert(sfetch_handle_valid(handle_));
  }

  ~AsyncBuffer() { sfetch_cancel(handle_); }

  static void FetchCallback(const sfetch_response_t* _reponse) {
    if (_reponse->dispatched) {
    }

    if (_reponse->fetched) {
      // Data has been loaded
      auto& buffer = from_user_data(_reponse).buffer_;

      // Append fetched data
      const std::byte* ptr = static_cast<const std::byte*>(_reponse->data.ptr);
      size_t size = _reponse->data.size;
      buffer.insert(buffer.end(), ptr, ptr + size);
    }

    if (_reponse->finished) {
      // Catch-all flag for when the request end

      if (_reponse->cancelled) {
        // User cancelled the request, BufferRequest is already deleted.
      } else if (_reponse->failed) {
        // Something went wrong
        auto& request = from_user_data(_reponse);
        request.status_ = Status::kFailed;

      } else {
        // Success !!
        auto& request = from_user_data(_reponse);
        request.status_ = Status::kSuccess;
      }

      // Free streaming buffer
      auto* buffer = sfetch_unbind_buffer(_reponse->handle);
      free(buffer);
    }
  }

  enum class Status {
    kSuccess,   // Async request completed with success
    kConsumed,  // Async request completed with success, consumed by caller with
                // a successful cal to get
    kPending,   // Request still pending
    kFailed,    // Request failed, will never end.
  };

  // Get the status and return the buffer.
  // It consumes the request in the sens that Status::kSuccess and the buffer
  // will on be return once.
  std::pair<Status, Buffer> Consume() {
    Buffer ret;
    if (status_ == Status::kSuccess) {
      std::swap(ret, buffer_);
      status_ = Status::kConsumed;
    }
    return {status_, std::move(ret)};
  }

 private:
  sfetch_range_t to_user_data() {
    void* user_data = this;
    return SFETCH_RANGE(user_data);
  }

  static AsyncBuffer& from_user_data(const sfetch_response_t* _reponse) {
    assert(!_reponse->cancelled &&
           "Request has already been deleted on cancel.");
    return **reinterpret_cast<AsyncBuffer**>(_reponse->user_data);
  }

  Status status_ = Status::kPending;
  Buffer buffer_;
  sfetch_handle_t handle_ = {};
};

}  // namespace

std::unique_ptr<AsyncBuffer> LoadAsync(const char* _filename) {
  auto request = std::make_unique<AsyncBuffer>(_filename);
  return request;
}

std::vector<std::byte> Load(const char* _filename) {
  auto r = LoadAsync(_filename);
  r->Consume();

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