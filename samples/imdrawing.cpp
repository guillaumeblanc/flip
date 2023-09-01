#include <cmath>

#include "flip/application.h"
#include "flip/imdraw.h"

// Implement the minimal flip::Application.
class ImDrawing : public flip::Application {
 public:
  ImDrawing() : flip::Application(Settings{.title = "ImDrawing"}) {}

  virtual bool Display(flip::Renderer& _renderer, float _time, float _dt,
                       float _inv_dt) override {
    auto transform = HMM_Rotate_RH(_time, HMM_Vec3{0, 1, 0}) *
                     HMM_Translate(HMM_Vec3{0, 3, 0}) *
                     HMM_Rotate_RH(HMM_PI / 4, HMM_Vec3{0, std::cos(_time),
                                                        std::sin(_time)});

    // Double face alpha blended white quad
    {
      auto drawer =
          flip::ImDraw{_renderer,
                       transform,
                       {.cull_mode = SG_CULLMODE_NONE, .blending = true}};

      sgl_begin_quads();
      sgl_c4b(0xff, 0xff, 0xff, 0xa0);

      sgl_v3f(-1, -1, 0);
      sgl_v3f(-1, 1, 0);
      sgl_v3f(1, 1, 0);
      sgl_v3f(1, -1, 0);
      sgl_v3f(-1, -1, 0);

      sgl_end();
    }

    // Green quad contour
    {
      auto drawer = flip::ImDraw{
          _renderer, transform * HMM_Rotate_RH(_time, HMM_Vec3{0, 1, 0}), {}};

      sgl_begin_line_strip();
      sgl_c4b(0x0, 0xff, 0x0, 0xff);

      sgl_v3f(-1, -1, 0);
      sgl_v3f(-1, 1, 0);
      sgl_v3f(1, 1, 0);
      sgl_v3f(1, -1, 0);
      sgl_v3f(-1, -1, 0);

      sgl_end();
    }

    return true;
  }
};

// Application instantiation function
std::unique_ptr<flip::Application> InstantiateApplication() {
  return std::make_unique<ImDrawing>();
}