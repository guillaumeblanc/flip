#pragma once

#include <utility>

#include "sokol/sokol_gfx.h"

template <typename _Id, void (*_Destroyer)(_Id)>
class SgResource {
 public:
  SgResource() : id_{SG_INVALID_ID} {}
  SgResource(_Id _id) : id_{_id} {}
  SgResource(const SgResource&) = delete;
  SgResource(SgResource&& _sr) { std::swap(id_, _sr.id_); }
  ~SgResource() {
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
  void reset(std::nullptr_t = nullptr) noexcept { *this = SgResource(); }

  operator bool() const noexcept { return id_.id != SG_INVALID_ID; }
  operator _Id() const noexcept { return id_; }

 private:
  _Id id_ = {};
};

using SgBuffer = SgResource<sg_buffer, &sg_destroy_buffer>;
using SgImage = SgResource<sg_image, &sg_destroy_image>;
using SgSampler = SgResource<sg_sampler, &sg_destroy_sampler>;
using SgShader = SgResource<sg_shader, &sg_destroy_shader>;
using SgPipeline = SgResource<sg_pipeline, &sg_destroy_pipeline>;
using SgPass = SgResource<sg_pass, &sg_destroy_pass>;