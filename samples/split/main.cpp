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

  virtual bool Display(flip::Renderer& _renderer) override {
    // A function to which renders the scene (a cube) in a specific color.
    auto draw_quad = [&_renderer](flip::Color _color) {
      const auto transform = HMM_Translate(HMM_Vec3{0, 6, 0}) *
                             HMM_Rotate_RH(1, HMM_Vec3{1, 1, 0}) *
                             HMM_Scale(HMM_Vec3{5, 5, 5});
      _renderer.DrawShapes({&transform, 1}, flip::Renderer::kCube, _color);
    };

    // Frame buffer size
    const int w = sapp_width(), h = sapp_height();

    if (divider_width_ * 2 <
        w) {  // Do not render if divider takes the whole space.

      // Renders left view
      sg_apply_scissor_rect(0, 0, (w - divider_width_) / 2, h, true);
      draw_quad(flip::kYellow);

      // Renders right view
      sg_apply_scissor_rect((w + divider_width_) / 2, 0,
                            (w - divider_width_) / 2, h, true);
      draw_quad(flip::kMagenta);

      // Restores full screen
      sg_apply_scissor_rect(0, 0, w, h, true);
    }

    return true;
  }

  virtual bool Menu() override {
    if (ImGui::BeginMenu("Sample")) {
      ImGui::SliderInt("Divider width", &divider_width_, 0, 1000, "%d",
                       ImGuiSliderFlags_Logarithmic);
      ImGui::EndMenu();
    }
    return true;
  }

 private:
  int divider_width_ = 0;
};

// Application instantiation function
std::unique_ptr<flip::Application> InstantiateApplication() {
  return std::make_unique<Split>();
}
