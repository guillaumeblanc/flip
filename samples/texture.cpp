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

    samplers_[false] = flip::MakeSgSampler(sg_sampler_desc{
        .min_filter = SG_FILTER_NEAREST, .mag_filter = SG_FILTER_NEAREST});
    samplers_[true] = flip::MakeSgSampler(sg_sampler_desc{
        .min_filter = SG_FILTER_LINEAR, .mag_filter = SG_FILTER_LINEAR});

    return true;
  }

  virtual bool Display(flip::Renderer& _renderer) override {
    // Double face alpha blended white quad
    {
      auto drawer =
          flip::ImDraw{_renderer,
                       HMM_M4D(1),
                       {.cull_mode = SG_CULLMODE_NONE, .blending = true}};

      if (texture_) {
        sgl_enable_texture();
        sgl_texture(image_, samplers_[linear_]);
      }

      sgl_begin_quads();

      sgl_c4f(color_.r, color_.g, color_.b, color_.a);

      // Vertex   x, y, z, u, v
      sgl_v3f_t2f(-5, 0, 0, 0, 1);
      sgl_v3f_t2f(-5, 10, 0, 0, 0);
      sgl_v3f_t2f(5, 10, 0, 1, 0);
      sgl_v3f_t2f(5, 0, 0, 1, 1);

      sgl_end();

      sgl_disable_texture();
    }

    return true;
  }

  virtual bool Menu() override {
    if (ImGui::BeginMenu("Sample")) {
      ImGui::Checkbox("Enable texture", &texture_);
      ImGui::Checkbox("Linear filtering", &linear_);
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
      sg_init_image(
          image,
          sg_image_desc{.width = width,
                        .height = height,
                        .pixel_format = SG_PIXELFORMAT_RGBA8,
                        .data = {.subimage = {{{.ptr = pixels,
                                                .size = static_cast<size_t>(
                                                    width * height * 4)}}}}});
      stbi_image_free(pixels);
    }

    // Returns image anyway, uses image state to know it's status
    return image;
  }

 private:
  flip::SgImage image_;
  flip::SgSampler samplers_[2];

  flip::Color color_ = flip::kYellow;
  bool texture_ = true;
  bool linear_ = true;
};

// Application instantiation function
std::unique_ptr<flip::Application> InstantiateApplication() {
  return std::make_unique<Texture>();
}