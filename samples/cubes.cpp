#include <cstring>
#include <memory>
#include <vector>

#include "flip/application.h"
#include "flip/renderer.h"
#include "hmm/HandmadeMath.h"
#include "imgui/imgui.h"

const char* kFlip[] = {
    "######### ###       ### ########",
    "########  ###       ### ##########",
    "###       ###           ###      ##",
    "###       ###           ###      ##",
    "###       ###           ###      ##",
    "######    ###       ### ##########",
    "#####     ###       ### ########",
    "###       ###       ### ###",
    "###       ###       ### ###",
    "###       ###       ### ###",
    "###       ###       ### ###",
    "###       ######### ### ###",
    "###       ######### ### ###",
};

// Uses draw and imgui features
class Cubes : public flip::Application {
 public:
  Cubes() : flip::Application(Settings{.title = "Cubes"}) {
    ComputeTransforms();
  }

 private:
  void ComputeTransforms() {
    transforms_.clear();

    // Letters
    const auto nlines = sizeof(kFlip) / sizeof(kFlip[0]);
    const float xoffset = std::strlen(kFlip[0]) / -2.f;
    const float yoffset = nlines - 1;
    auto pos = HMM_Vec3{xoffset, yoffset, 0};
    for (auto line = kFlip; line < kFlip + nlines;
         ++line, pos.X = xoffset, pos.Y -= 1) {
      for (auto c = *line; *c; ++c, pos.X += 1) {
        if (*c == '#') {
          transforms_.push_back(HMM_Translate(pos) * HMM_Scale(scale_));
        }
      }
    }

    // Floor
    if (show_floor_) {
      transforms_.push_back(HMM_Scale(HMM_Vec3{50, 1, 50}) *
                            HMM_Translate(HMM_Vec3{-.5f, -1.f, -.5f}));
    }
  }

  virtual bool Display(flip::Renderer& _renderer) override {
    return _renderer.DrawShapes(transforms_, flip::Renderer::kCube);
  }

  virtual bool Menu() override {
    if (ImGui::BeginMenu("Sample")) {
      bool recompute = false;
      recompute |= ImGui::SliderFloat3("Scale", scale_.Elements, .1f, 5.f);
      recompute |= ImGui::Checkbox("Show floor", &show_floor_);

      if (recompute) {
        ComputeTransforms();
      }
      ImGui::EndMenu();
    }

    return true;
  }

  std::vector<HMM_Mat4> transforms_;
  HMM_Vec3 scale_ = {.8f, .8f, 3.f};
  bool show_floor_ = true;
};

std::unique_ptr<flip::Application> InstantiateApplication() {
  return std::make_unique<Cubes>();
}
