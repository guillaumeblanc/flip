#pragma once

#include <span>
#include <utility>

#include "sokol/sokol_gfx.h"

namespace flip {

// unique_ptr like sokol resource object.
template <typename _Id, void (*_Destroyer)(_Id)>
class SgResource {
 public:
  SgResource() noexcept : id_{SG_INVALID_ID} {}
  explicit SgResource(_Id _id) noexcept : id_{_id} {}
  SgResource(const SgResource&) = delete;
  SgResource(SgResource&& _sr) noexcept { std::swap(id_, _sr.id_); }
  ~SgResource() noexcept {
    if (id_.id != SG_INVALID_ID) {
      _Destroyer(id_);
    }
  }

  _Id release() noexcept {
    _Id id = id_;
    id_ = SG_INVALID_ID;
    return id;
  }

  SgResource& operator=(SgResource&& _sr) noexcept {
    std::swap(id_, _sr.id_);
    return *this;
  }
  const SgResource& operator=(const SgResource&) = delete;

  SgResource& operator=(std::nullptr_t) noexcept {
    reset(nullptr);
    return *this;
  }

  void reset(_Id _id = _Id{SG_INVALID_ID}) noexcept { *this = SgResource(_id); }

  operator bool() const noexcept { return id_.id != SG_INVALID_ID; }
  operator _Id() const noexcept { return id_; }

 private:
  _Id id_ = {};
};

using SgBuffer = SgResource<sg_buffer, &sg_destroy_buffer>;
inline SgBuffer MakeSgBuffer(const sg_buffer_desc& _desc) {
  return SgBuffer(sg_make_buffer(&_desc));
}
using SgImage = SgResource<sg_image, &sg_destroy_image>;
inline SgImage MakeSgImage(const sg_image_desc& _desc) {
  return SgImage(sg_make_image(&_desc));
}
using SgSampler = SgResource<sg_sampler, &sg_destroy_sampler>;
inline SgSampler MakeSgSampler(const sg_sampler_desc& _desc) {
  return SgSampler(sg_make_sampler(&_desc));
}
using SgShader = SgResource<sg_shader, &sg_destroy_shader>;
inline SgShader MakeSgShader(const sg_shader_desc& _desc) {
  return SgShader(sg_make_shader(&_desc));
}
using SgPipeline = SgResource<sg_pipeline, &sg_destroy_pipeline>;
inline SgPipeline MakeSgPipeline(const sg_pipeline_desc& _desc) {
  return SgPipeline(sg_make_pipeline(&_desc));
}
using SgPass = SgResource<sg_pass, &sg_destroy_pass>;
inline SgPass MakeSgPass(const sg_pass_desc& _desc) {
  return SgPass(sg_make_pass(&_desc));
}

// Represent a buffer with that can be partial (ex dynamic/appended).
struct BufferBinding {
  sg_buffer id;
  int offset;
};

// Dynamic buffer, which size will stabilize to the minimum required each frame.
class SgDynamicBuffer {
 public:
  BufferBinding Append(std::span<const std::byte> _data);

 private:
  SgBuffer buffer_;
};

}  // namespace flip