#pragma once

#include <unordered_map>
#include <vector>

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
  void End(sg_image _image, bool _linear);

  void Submit(const ImDraw::Vertex& _vertex) { vertices_.push_back(_vertex); }

 protected:
 private:
  std::unordered_map<ImMode, SgPipeline, ModeHash> pipelines_;
  SgShader shader_;

  std::vector<ImDraw::Vertex> vertices_;
  SgDynamicBuffer buffer_;

  SgImage image_;
  SgSampler samplers_[2];  // nearest & linear
};

};  // namespace flip