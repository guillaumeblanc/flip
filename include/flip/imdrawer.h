#pragma once

#include <unordered_map>

// Explicitly exposes sokol includes and data structures which are supposed to
// be used directly from user side
#include "flip/utils/sokol_gfx.h"
#include "hmm/HandmadeMath.h"
#include "sokol/util/sokol_gl.h"

namespace flip {

struct ImMode {
  // Z
  bool z_write = true;
  sg_compare_func z_compare = SG_COMPAREFUNC_LESS_EQUAL;

  // Culling
  sg_cull_mode cull_mode = SG_CULLMODE_BACK;

  // Blending
  bool blending = false;
};

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