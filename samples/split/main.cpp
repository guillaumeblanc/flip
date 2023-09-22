#include "flip/application.h"
#include "flip/math.h"
#include "flip/renderer.h"
#include "flip/utils/sokol_gfx.h"
#include "imgui/imgui.h"
#include "sokol/sokol_app.h"

// Implement a split screen flip::Application.
class Split : public flip::Application {
 public:
  Split() : flip::Application(Settings{.title = "Split"}) {}

 private:
  virtual bool Display(flip::Renderer& _renderer) override {
    // A function to which renders the scene (a cube) in a specific color.
    auto draw = [&_renderer](flip::Color _color) {
      const auto transform = HMM_Translate(HMM_Vec3{0, 6, 0}) *
                             HMM_Rotate_RH(1, HMM_Vec3{1, 1, 0}) *
                             HMM_Scale(HMM_Vec3{5, 5, 5});
      _renderer.DrawShapes({&transform, 1}, flip::Renderer::kCube, _color);
    };

    auto [screen, left, div, right] = splits();

    {  // Renders left view
      sg_apply_scissor_rect(left.x, left.y, left.w, left.h, true);
      draw(flip::kYellow);
    }

    {  // Renders right view
      sg_apply_scissor_rect(right.x, right.y, right.w, right.h, true);
      draw(flip::kMagenta);
    }

    // Restores full screen
    sg_apply_scissor_rect(0, 0, screen.w, screen.h, true);

    return true;
  }

  virtual bool Menu() override {
    if (ImGui::BeginMenu("Sample")) {
      ImGui::SliderInt("Divider width", &divider_width_, 0, 20);
      ImGui::SliderFloat("Divider position", &divider_pos_, 0, 1, "%2g");
      ImGui::EndMenu();
    }
    return true;
  }

  struct Splits {
    struct {
      int w, h;
    } screen;
    struct Rect {
      int x, y, w, h;
    } left, divider, right;
  };

  Splits splits(int _width = sapp_width(), int _height = sapp_height()) const {
    auto div_l = std::max(
        static_cast<int>(_width * divider_pos_) - divider_width_ / 2, 0);
    auto div_r = std::min(div_l + divider_width_, _width);
    assert(div_l >= 0 && div_l <= _width && div_r >= 0 && div_r <= _width);

    return {{_width, _height},
            {0, 0, div_l, _height},
            {div_l, 0, divider_width_, _height},
            {div_r, 0, _width - div_r, _height}};
  }

  int divider_width_ = 1;    // Width (pixels)
  float divider_pos_ = .5f;  // Position (screen ratio)
};

// Application instantiation function
std::unique_ptr<flip::Application> InstantiateApplication() {
  return std::make_unique<Split>();
}
