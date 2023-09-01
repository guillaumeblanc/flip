#pragma once
#include <array>

#include "flip/renderer.h"
#include "flip/utils/sokol_gfx.h"

namespace flip {

// Base Renderer interface
class Shapes {
 public:
  Shapes() = default;
  ~Shapes() = default;

  bool Initialize();

  bool Draw(Renderer::Shape _shape, Color _color, int _intances,
            const BufferBinding& _models, HMM_Mat4& _view_proj);

 protected:
 private:
  SgShader shader_;
  SgPipeline pipeline_;

  // One vertex/index-buffer-pair for all shapes
  SgBuffer vertex_buffer_;
  SgBuffer index_buffer_;

  // Pairs of base offset and num elements.
  std::array<std::pair<int, int>, Renderer::Shape::kCount> draws_;
};

}  // namespace flip