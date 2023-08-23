#pragma once
#include <array>

#include "flip/renderer.h"
#include "sokol_gfx_utils.h"

namespace flip {

// Base Renderer interface
class Shapes {
 public:
  Shapes() = default;
  ~Shapes() = default;

  bool Initialize();

  bool Draw(flip::Renderer::Shape _shape, int _intances,
            const sg_buffer& transforms, HMM_Mat4& _view_proj);

 protected:
 private:
  SgShader shader_;
  SgPipeline pipeline_;

  // One vertex/index-buffer-pair for all shapes
  SgBuffer vertex_buffer_;
  SgBuffer index_buffer_;

  // Pairs of base offset and num elements.
  std::array<std::pair<int, int>, 5> draws_;
};

}  // namespace flip