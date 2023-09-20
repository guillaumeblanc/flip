#include <cstring>
#include <memory>
#include <vector>

#include "flip/application.h"
#include "flip/math.h"
#include "flip/renderer.h"
#include "imgui/imgui.h"
#include "logo.h"

// Uses draw and imgui features
class Shapes : public flip::Application {
 public:
  Shapes() : flip::Application(Settings{.title = "Shapes"}) {
    ComputeTransforms();
  }

 private:
  // Decompresses RLE logo into an array of transforms, where each transform
  // maps a pixel.
  void ComputeTransforms() {
    transforms_.clear();

    const float kShapeSize = .1f;
    const float xoffset = -flip::logo::kWidth * kShapeSize / 2.f;
    const float yoffset = flip::logo::kHeight * kShapeSize;

    auto pos = HMM_Vec3{xoffset, yoffset, 0};
    auto pixels = flip::logo::kPixels;
    auto end = flip::logo::kPixels + sizeof(flip::logo::kPixels);
    for (; pixels < end; ++pixels) {
      if (*pixels == 0) {  // Next line
        pos = HMM_Vec3{xoffset, pos.Y - kShapeSize, 0};
        continue;
      }
      const int count = *pixels & 0x7f;
      if (*pixels & 0x80) {  // Pixels on
        for (int c = 0; c < count; ++c, pos.X += kShapeSize) {
          transforms_.push_back(HMM_Translate(pos) *
                                HMM_Scale(scale_ * kShapeSize));
        }
      } else {  // Pixels off
        pos.X += kShapeSize * count;
      }
    }
  }

  // Renders a box per transform
  virtual bool Display(flip::Renderer& _renderer) override {
    return _renderer.DrawShapes(transforms_, shape_, color_);
  }

  virtual bool Menu() override {
    if (ImGui::BeginMenu("Sample")) {
      bool recompute = false;
      recompute |= ImGui::SliderFloat3("Scale", scale_.Elements, .1f, 100.f,
                                       "%.2g", ImGuiSliderFlags_Logarithmic);
      if (recompute) {
        ComputeTransforms();
      }

      int shape = static_cast<int>(shape_);
      ImGui::Combo("Shape type", &shape,
                   "Plane\0Cube\0Sphere\0Cylinder\0Torus\0");
      shape_ = static_cast<flip::Renderer::Shape>(shape);

      ImGui::ColorPicker3("Shape color", color_.rgba);

      ImGui::EndMenu();
    }

    return true;
  }

  std::vector<HMM_Mat4> transforms_;
  flip::Renderer::Shape shape_ = flip::Renderer::Shape::kSphere;
  HMM_Vec3 scale_ = {.8f, .8f, .8f};
  flip::Color color_ = flip::kWhite;
};

std::unique_ptr<flip::Application> InstantiateApplication() {
  return std::make_unique<Shapes>();
}
