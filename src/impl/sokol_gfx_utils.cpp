#include "sokol_gfx_utils.h"

BufferBinding SgDynamicBuffer::Append(std::span<const std::byte> _data) {
  if (!buffer_ || sg_query_buffer_will_overflow(buffer_, _data.size_bytes())) {
    // Reallocates the buffer to be able to store enough data for the frame.
    // This shall happens until a stable maximum size (per frame) is reached.
    // sokol manages frame counter internally.
    const auto& info = sg_query_buffer_info(buffer_);
    buffer_ = MakeSgBuffer(
        sg_buffer_desc{.size = info.append_pos + _data.size_bytes(),
                       .usage = SG_USAGE_STREAM,
                       .label = "flip:: dynamic buffer"});
  }
  return {
      .id = buffer_,
      .offset = sg_append_buffer(
          buffer_, sg_range{.ptr = _data.data(), .size = _data.size_bytes()})};
}
