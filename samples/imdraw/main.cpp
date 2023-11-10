#include <cmath>

#include "flip/application.h"
#include "flip/imdraw.h"
#include "flip/utils/time.h"

// Implement the minimal flip::Application.
class ImDraw : public flip::Application {
 public:
  ImDraw() : flip::Application(Settings{.title = "ImDraw"}) {}

 private:
  virtual LoopControl Update(const flip::Time& _time) override {
    auto elapsed = _time.elapsed;
    transform1_ = HMM_Rotate_RH(elapsed, HMM_Vec3{0, 1, 0}) *
                  HMM_Translate(HMM_Vec3{0, 6, 0}) *
                  HMM_Rotate_RH(HMM_PI / 4, HMM_Vec3{std::cos(elapsed),
                                                     std::sin(elapsed), 0}) *
                  HMM_Scale(HMM_Vec3{3, 3, 3});
    transform2_ = transform1_ * HMM_Rotate_RH(elapsed, HMM_Vec3{0, 1, 0});

    return LoopControl::kContinue;
  }

  virtual bool Display(flip::Renderer& _renderer) override {
    // Green quad contour
    {
      auto drawer = flip::ImDraw{
          _renderer, transform2_, {.type = SG_PRIMITIVETYPE_LINE_STRIP}};

      drawer.color(flip::kGreen);

      drawer.vertex(-1, -1, 0);
      drawer.vertex(-1, 1, 0);
      drawer.vertex(1, 1, 0);
      drawer.vertex(1, -1, 0);
      drawer.vertex(-1, -1, 0);
    }

    // Double face alpha blended white quad
    {
      auto drawer = flip::ImDraw{_renderer,
                                 transform1_,
                                 {.type = SG_PRIMITIVETYPE_TRIANGLE_STRIP,
                                  .cull_mode = SG_CULLMODE_NONE,
                                  .alpha_blending = true}};

      drawer.color(flip::kYellow, .7f);

      drawer.vertex(-1, 1, 0);
      drawer.vertex(-1, -1, 0);
      drawer.vertex(1, 1, 0);
      drawer.vertex(1, -1, 0);
    }

    return true;
  }

  HMM_Mat4 transform1_;
  HMM_Mat4 transform2_;
};

// Application instantiation function
std::unique_ptr<flip::Application> InstantiateApplication() {
  return std::make_unique<ImDraw>();
}
