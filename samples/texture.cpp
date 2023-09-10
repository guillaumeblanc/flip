#include "flip/application.h"
#include "flip/imdraw.h"
#include "flip/utils/loader.h"
#include "flip/utils/sokol_gfx.h"
#include "imgui/imgui.h"

// Implement the minimal flip::Application.
class Texture : public flip::Application {
 public:
  Texture() : flip::Application(Settings{.title = "Texture"}) {}

 protected:
  virtual bool Initialize(bool _headless) override {
    if (_headless) {
      return true;
    }

    image_ = flip::LoadImage(flip::Load("media/texture.png"));
    sampler_ = SetupSampler(linear_);

    return true;
  }

  virtual bool Display(flip::Renderer& _renderer) override {
    auto drawer = flip::ImDraw{_renderer,
                               flip::kIdentity4,
                               {.type = SG_PRIMITIVETYPE_TRIANGLE_STRIP,
                                .cull_mode = SG_CULLMODE_NONE,
                                .blending = true}};

    if (texture_) {
      drawer.texture(image_.id(), sampler_.id());
    }

    drawer.color(color_);

    drawer.vertex(-5, 10, 0, 0, 0);
    drawer.vertex(-5, 0, 0, 0, 1);
    drawer.vertex(5, 10, 0, 1, 0);
    drawer.vertex(5, 0, 0, 1, 1);

    return true;
  }

  static flip::SgSampler SetupSampler(bool _linear) {
    auto filter = _linear ? SG_FILTER_LINEAR : SG_FILTER_NEAREST;
    return flip::MakeSgSampler(sg_sampler_desc{
        .min_filter = filter, .mag_filter = filter, .label = "Texture sample"});
  }

  virtual bool Menu() override {
    if (ImGui::BeginMenu("Sample")) {
      ImGui::Checkbox("Enable texture", &texture_);
      if (ImGui::Checkbox("Linear filtering", &linear_)) {
        sampler_ = SetupSampler(linear_);
      }
      ImGui::ColorEdit4("Color", color_.rgba);
      ImGui::EndMenu();
    }

    return true;
  }

 private:
  flip::SgImage image_;
  flip::SgSampler sampler_;

  flip::Color color_ = flip::kYellow;
  bool texture_ = true;
  bool linear_ = true;
};

// Application instantiation function
std::unique_ptr<flip::Application> InstantiateApplication() {
  return std::make_unique<Texture>();
}
