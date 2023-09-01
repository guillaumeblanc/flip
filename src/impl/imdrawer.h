#pragma once

#include <unordered_map>

#include "flip/imdraw.h"

namespace flip {

inline bool operator==(ImMode const& _a, ImMode const& _b) noexcept {
  return _a.z_write == _b.z_write && _a.z_compare == _b.z_compare &&
         _a.cull_mode == _b.cull_mode && _a.blending == _b.blending;
}

struct ModeHash {
  std::size_t operator()(ImMode const& _mode) const noexcept {
    auto hash =
        std::size_t(_mode.z_write) << 0 | std::size_t(_mode.z_compare) << 1 |
        std::size_t(_mode.cull_mode) << 10 | std::size_t(_mode.blending) << 12;
    return hash;
  }
};

class ImDrawer {
 public:
  ImDrawer();
  virtual ~ImDrawer();

  void Begin(const HMM_Mat4& _view_proj, const HMM_Mat4& _transform,
             const ImMode& _mode);
  void End();

 protected:
 private:
  using SglPipeline = SgResource<sgl_pipeline, sgl_destroy_pipeline>;

  std::unordered_map<ImMode, SglPipeline, ModeHash> pipelines_;
};

};  // namespace flip