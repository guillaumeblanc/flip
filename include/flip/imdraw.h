#pragma once

// Explicitly exposes sokol includes and data structures which are supposed to
// be used directly from user side
#include "flip/renderer.h"
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

// RAII to begin/end the default rendering pass
class ImDraw {
 public:
  ImDraw(Renderer& _renderer, const HMM_Mat4& _transform, const ImMode& _mode)
      : renderer_(_renderer) {
    renderer_.BeginImDraw(_transform, _mode);
  }
  ~ImDraw() { renderer_.EndImDraw(); }

 private:
  Renderer& renderer_;
};

};  // namespace flip