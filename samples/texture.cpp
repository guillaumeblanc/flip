#include <fstream>
#include <span>
#include <vector>

#include "flip/application.h"
#include "flip/imdraw.h"
#include "flip/utils/sokol_gfx.h"
#include "imgui/imgui.h"
#include "stb/stb_image.h"

// Implement the minimal flip::Application.
class Texture : public flip::Application {
 public:
  Texture() : flip::Application(Settings{.title = "Texture"}) {}

 protected:
  virtual bool Initialize(bool _headless) override {
    if (_headless) {
      return true;
    }

    image_ = LoadImage("media/flip_512.png");

    sampler_ = SetupSampler(linear_);

    return true;
  }

  virtual bool Display(flip::Renderer& _renderer) override {
    auto drawer = flip::ImDraw{_renderer,
                               HMM_M4D(1),
                               {.type = SG_PRIMITIVETYPE_TRIANGLE_STRIP,
                                .cull_mode = SG_CULLMODE_NONE,
                                .blending = true}};

    if (texture_) {
      drawer.texture(image_, sampler_);
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
  flip::SgImage LoadImage(const char* _filename) {
    auto file = std::ifstream(_filename, std::ios::binary);
    if (!file.is_open()) {
      return {};
    }
    auto content = std::vector<char>(std::istreambuf_iterator<char>(file),
                                     std::istreambuf_iterator<char>());

    return LoadImage(std::as_bytes(std::span{content}));
  }

  flip::SgImage LoadImage(std::span<const std::byte> _buffer) {
    auto image = flip::SgImage{sg_alloc_image()};

    int width, height, channels;
    stbi_uc* pixels = stbi_load_from_memory(
        reinterpret_cast<const stbi_uc*>(_buffer.data()),
        static_cast<int>(_buffer.size_bytes()), &width, &height, &channels, 4);
    if (!pixels) {
      // Failed to load image from buffer
      const char* error = stbi_failure_reason();
    } else {
      // Init image from pixels
      sg_init_image(image,
                    sg_image_desc{
                        .width = width,
                        .height = height,
                        .pixel_format = SG_PIXELFORMAT_RGBA8,
                        .data = {.subimage = {{{.ptr = pixels,
                                                .size = static_cast<size_t>(
                                                    width * height * 4)}}}},
                        .label = "Texture sample",
                    });
      stbi_image_free(pixels);
    }

    // Returns image anyway, uses image state to know it's status
    return image;
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
