#include <cstring>
#include <memory>
#include <vector>

#include "flip/application.h"
#include "flip/math.h"
#include "flip/renderer.h"
#include "flip_128.h"
#include "imgui/imgui.h"

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

    const float kBoxSize = .1f;
    const float xoffset = -flip::logo::kWidth * kBoxSize / 2.f;
    const float yoffset = flip::logo::kHeight * kBoxSize;

    auto pos = HMM_Vec3{xoffset, yoffset, 0};
    auto pixels = flip::logo::kPixels;
    auto end = flip::logo::kPixels + sizeof(flip::logo::kPixels);
    for (; pixels < end; ++pixels) {
      if (*pixels == 0) {  // Next line
        pos = HMM_Vec3{xoffset, pos.Y - kBoxSize, 0};
        continue;
      }
      const int count = *pixels & 0x7f;
      if (*pixels & 0x80) {  // Pixels on
        for (int c = 0; c < count; ++c, pos.X += kBoxSize) {
          transforms_.push_back(HMM_Translate(pos) *
                                HMM_Scale(scale_ * kBoxSize) *
                                HMM_Translate(HMM_Vec3{.5f, .5f, .5f}));
        }
      } else {  // Pixels off
        pos.X += kBoxSize * count;
      }
    }
  }

  // Renders a box per transform
  virtual bool Display(flip::Renderer& _renderer) override {
    return _renderer.DrawShapes(transforms_, shape_, flip::kWhite);
  }

  virtual bool Menu() override {
    if (ImGui::BeginMenu("Sample")) {
      bool recompute = false;
      recompute |= ImGui::SliderFloat3("Scale", scale_.Elements, .1f, 2.f);
      if (recompute) {
        ComputeTransforms();
      }

      int shape = static_cast<int>(shape_);
      ImGui::Combo("Shape type", &shape,
                   "Plane\0Cube\0Sphere\0Cylinder\0Torus\0");
      shape_ = static_cast<flip::Renderer::Shape>(shape);

      ImGui::EndMenu();
    }

    return true;
  }

  std::vector<HMM_Mat4> transforms_;
  flip::Renderer::Shape shape_ = flip::Renderer::Shape::kSphere;
  HMM_Vec3 scale_ = {.8f, .8f, .8f};
};

std::unique_ptr<flip::Application> InstantiateApplication() {
  return std::make_unique<Shapes>();
}
