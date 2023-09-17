#pragma once

// Explicitly exposes sokol includes and data structures which are supposed to
// be used directly from user side
#include <vector>

#include "flip/math.h"
#include "flip/renderer.h"
#include "flip/utils/sokol_gfx.h"

namespace flip {

struct ImMode {
  // Primitive type
  sg_primitive_type type = SG_PRIMITIVETYPE_TRIANGLES;

  // Z
  bool z_write = true;
  sg_compare_func z_compare = SG_COMPAREFUNC_LESS_EQUAL;

  // Culling
  sg_cull_mode cull_mode = SG_CULLMODE_BACK;

  // Blending
  bool blending = false;
};

struct ImVertex {
  HMM_Vec3 position = {0, 0, 0};
  Color color = {1, 1, 1, 1};
  HMM_Vec2 uv = {0, 0};
};

// RAII to begin/end an immediate mode draw
class ImDraw {
 public:
  ImDraw(Renderer& _renderer, const HMM_Mat4& _transform, const ImMode& _mode)
      : renderer_(_renderer) {
    renderer_.BeginImDraw(_transform, _mode);
  }
  ~ImDraw() { renderer_.EndImDraw(vertices_, image_, sampler_); }

  void texture(sg_image _image, sg_sampler _sampler) {
    image_ = _image;
    sampler_ = _sampler;
  }

  // Modifies current (internal) vertex
  void uv(const HMM_Vec2& _uv) { vertex_.uv = _uv; }
  void uv(float _u, float _v) { vertex_.uv = {_u, _v}; }
  void color(const Color& _color) { vertex_.color = _color; }
  void color(const Color& _color, float _a) {
    vertex_.color = {_color.r, _color.g, _color.b, _a};
  }
  void color(float _r, float _g, float _b, float _a) {
    vertex_.color = {_r, _g, _b, _a};
  }

  // Modifies current (internal) vertex and submit point.
  void vertex(const HMM_Vec3& _xyz) {
    vertex_.position = _xyz;
    Submit();
  }
  void vertex(float _x, float _y, float _z) {
    vertex_.position = {_x, _y, _z};
    Submit();
  }

  void vertex(const HMM_Vec3& _xyz, const Color& _color) {
    vertex_.position = _xyz;
    vertex_.color = _color;
    Submit();
  }

  void vertex(float _x, float _y, float _z, float _r, float _g, float _b,
              float _a) {
    vertex_.position = {_x, _y, _z};
    vertex_.color = {_r, _g, _b, _a};
    Submit();
  }

  void vertex(float _x, float _y, float _z, float _u, float _v) {
    vertex_.position = {_x, _y, _z};
    vertex_.uv = {_u, _v};
    Submit();
  }

  void vertex(const HMM_Vec3& _xyz, const HMM_Vec2& _uv) {
    vertex_.position = _xyz;
    vertex_.uv = _uv;
    Submit();
  }

  void vertex(const HMM_Vec3& _xyz, const HMM_Vec2& _uv, const Color& _color) {
    vertex_.position = _xyz;
    vertex_.uv = _uv;
    vertex_.color = _color;
    Submit();
  }

  void vertex(float _x, float _y, float _z, float _u, float _v, float _r,
              float _g, float _b, float _a) {
    vertex_.position = {_x, _y, _z};
    vertex_.uv = {_u, _v};
    vertex_.color = {_r, _g, _b, _a};
    Submit();
  }

 private:
  // Submit new vertex for the primitive
  void Submit() { vertices_.push_back(vertex_); }

  Renderer& renderer_;

  std::vector<ImVertex> vertices_;
  ImVertex vertex_;
  sg_image image_ = {SG_INVALID_ID};
  sg_sampler sampler_ = {SG_INVALID_ID};
};

};  // namespace flip