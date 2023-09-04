#pragma once

#include <unordered_map>

#include "flip/imdraw.h"

namespace flip {

inline bool operator==(ImMode const& _a, ImMode const& _b) noexcept {
  return _a.type == _b.type && _a.z_write == _b.z_write &&
         _a.z_compare == _b.z_compare && _a.cull_mode == _b.cull_mode &&
         _a.blending == _b.blending;
}

struct ModeHash {
  std::size_t operator()(ImMode const& _mode) const noexcept {
    auto hash = std::size_t(_mode.type) << 0 | std::size_t(_mode.z_write) << 3 |
                std::size_t(_mode.z_compare) << 4 |
                std::size_t(_mode.cull_mode) << 13 |
                std::size_t(_mode.blending) << 15;
    return hash;
  }
};

class ImDrawer {
 public:
  ImDrawer();
  virtual ~ImDrawer();

  void Begin(const HMM_Mat4& _view_proj, const HMM_Mat4& _transform,
             const ImMode& _mode);
  void End(std::span<const ImVertex> _vertices, sg_image _image,
           sg_sampler _sampler);

 protected:
 private:
  std::unordered_map<ImMode, SgPipeline, ModeHash> pipelines_;
  SgShader shader_;

  SgDynamicBuffer buffer_;

  SgImage image_;
  SgSampler sampler_;  // nearest & linear
};

};  // namespace flip