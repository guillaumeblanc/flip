#include "imgui.h"

#include "imgui/imgui.h"
#include "imgui_font.h"
#include "sokol/sokol_app.h"
#include "sokol/sokol_gfx.h"
#include "sokol/util/sokol_imgui.h"

namespace flip {

Imgui::Imgui() {
  const auto& app_desc = sapp_query_desc();

  // Setup sokol-imgui
  auto simgui_desc = simgui_desc_t{
      .sample_count = app_desc.sample_count,
      .no_default_font = true,
      .logger = {app_desc.logger.func, app_desc.logger.user_data}};
  simgui_setup(simgui_desc);

  // Style
  auto& style = ImGui::GetStyle();
  style.WindowBorderSize = 1.f;
  style.FrameBorderSize = 1.f;
  style.FrameRounding = 2.f;
  style.FramePadding = ImVec2{4, 0};

  // Configuration
  auto& io = ImGui::GetIO();
  // ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  // Configures Dear ImGui with our own embedded font.
  ImFontConfig font_cfg;
  font_cfg.FontDataOwnedByAtlas = false;
  font_cfg.OversampleH = 2;
  font_cfg.OversampleV = 2;
  font_cfg.RasterizerMultiply = 1.5f;
  io.Fonts->AddFontFromMemoryTTF(KFontTTF, sizeof(KFontTTF), 16.0f, &font_cfg);

  // Creates font texture and linear-filtering sampler for the custom font
  unsigned char* font_pixels;
  int font_width, font_height;
  io.Fonts->GetTexDataAsRGBA32(&font_pixels, &font_width, &font_height);
  sg_image_desc img_desc = {
      .width = font_width,
      .height = font_height,
      .pixel_format = SG_PIXELFORMAT_RGBA8,
      .label = "flip:: imgui font",
  };
  img_desc.data.subimage[0][0].ptr = font_pixels;
  img_desc.data.subimage[0][0].size = font_width * font_height * 4;
  font_image_ = MakeSgImage(img_desc);
  font_sampler_ = MakeSgSampler(sg_sampler_desc{
      .min_filter = SG_FILTER_LINEAR,
      .mag_filter = SG_FILTER_LINEAR,
      .wrap_u = SG_WRAP_CLAMP_TO_EDGE,
      .wrap_v = SG_WRAP_CLAMP_TO_EDGE,
      .label = "flip:: imgui font",
  });
  auto font_desc = simgui_image_desc_t{
      .image = font_image_.id(),
      .sampler = font_sampler_.id(),
  };
  io.Fonts->TexID = simgui_imtextureid(simgui_make_image(&font_desc));
}

Imgui::~Imgui() {
  font_image_.reset();
  font_sampler_.reset();
  simgui_shutdown();
}

bool Imgui::Event(const sapp_event& _event) {
  return simgui_handle_event(&_event);
}

void Imgui::BeginFrame() {
  simgui_new_frame(simgui_frame_desc_t{.width = sapp_width(),
                                       .height = sapp_height(),
                                       .delta_time = sapp_frame_duration(),
                                       .dpi_scale = sapp_dpi_scale()});

  // Open a main menu bar by default
  bool open = ImGui::BeginMainMenuBar();
  assert(open);
}
void Imgui::EndFrame() {
  ImGui::EndMainMenuBar();
  simgui_render();
}

}  // namespace flip