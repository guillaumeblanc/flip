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
  const auto theme_d = ImVec4(.41f, .41f, .0f, 1.f);
  const auto theme = ImVec4(.61f, .61f, .0f, 1.f);
  const auto theme_l = ImVec4(.81f, .81f, .0f, 1.f);
  style.Colors[ImGuiCol_Text] = ImVec4(1.f, 1.f, 1.f, 1.f);
  style.Colors[ImGuiCol_TextDisabled] = ImVec4(.5f, .5f, .5f, 1.f);
  style.Colors[ImGuiCol_WindowBg] = ImVec4(.13f, .14f, .15f, .7f);
  style.Colors[ImGuiCol_ChildBg] = ImVec4(.13f, .14f, .15f, .7f);
  style.Colors[ImGuiCol_PopupBg] = ImVec4(.13f, .14f, .15f, 1.f);
  style.Colors[ImGuiCol_Border] = ImVec4(.43f, .43f, .5f, .5f);
  style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.f, 0.f, 0.f, 1.f);
  style.Colors[ImGuiCol_FrameBg] = ImVec4(.25f, .25f, .25f, 1.f);
  style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(.38f, .38f, .38f, 1.f);
  style.Colors[ImGuiCol_FrameBgActive] = ImVec4(.67f, .67f, .67f, .39f);
  style.Colors[ImGuiCol_TitleBg] = ImVec4(.08f, .08f, .09f, .9f);
  style.Colors[ImGuiCol_TitleBgActive] = theme_d;
  style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.f, 0.f, 0.f, .51f);
  style.Colors[ImGuiCol_MenuBarBg] = ImVec4(.14f, .14f, .14f, .8f);
  style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(.02f, .02f, .02f, .53f);
  style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(.31f, .31f, .31f, 1.f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered] = theme;
  style.Colors[ImGuiCol_ScrollbarGrabActive] = theme_l;
  style.Colors[ImGuiCol_CheckMark] = theme_l;
  style.Colors[ImGuiCol_SliderGrab] = theme;
  style.Colors[ImGuiCol_SliderGrabActive] = theme_l;
  style.Colors[ImGuiCol_Button] = ImVec4(.25f, .25f, .25f, 1.f);
  style.Colors[ImGuiCol_ButtonHovered] = theme_d;
  style.Colors[ImGuiCol_ButtonActive] = theme_l;
  style.Colors[ImGuiCol_Header] = ImVec4(.22f, .22f, .22f, 1.f);
  style.Colors[ImGuiCol_HeaderHovered] = theme_d;
  style.Colors[ImGuiCol_HeaderActive] = theme_l;
  style.Colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_Border];
  style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(.41f, .42f, .44f, 1.f);
  style.Colors[ImGuiCol_SeparatorActive] = ImVec4(.26f, .59f, .98f, .95f);
  style.Colors[ImGuiCol_ResizeGrip] = style.Colors[ImGuiCol_Border];
  style.Colors[ImGuiCol_ResizeGripHovered] = theme;
  style.Colors[ImGuiCol_ResizeGripActive] = theme_l;
  style.Colors[ImGuiCol_Tab] = ImVec4(.08f, .08f, .09f, .83f);
  style.Colors[ImGuiCol_TabHovered] = ImVec4(.33f, .34f, .36f, .83f);
  style.Colors[ImGuiCol_TabActive] = ImVec4(.23f, .23f, .24f, 1.f);
  style.Colors[ImGuiCol_TabUnfocused] = ImVec4(.08f, .08f, .09f, 1.f);
  style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(.13f, .14f, .15f, 1.f);
  // style.Colors[ImGuiCol_DockingPreview] = ImVec4(.26f, .59f, .98f, .7f);
  // style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(.2f, .2f, .2f, 1.f);
  style.Colors[ImGuiCol_PlotLines] = theme_l;
  style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.f, .43f, .35f, 1.f);
  style.Colors[ImGuiCol_PlotHistogram] = theme_l;
  style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.f, .6f, 0.f, 1.f);
  style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(.26f, .59f, .98f, .35f);
  style.Colors[ImGuiCol_DragDropTarget] = ImVec4(.11f, .64f, .92f, 1.f);
  style.Colors[ImGuiCol_NavHighlight] = ImVec4(.26f, .59f, .98f, 1.f);
  style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.f, 1.f, 1.f, .7f);
  style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(.8f, .8f, .8f, .2f);
  style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(.8f, .8f, .8f, .35f);
  style.WindowBorderSize = 1.f;
  style.FrameBorderSize = 1.f;
  style.GrabRounding = 2.f;
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
  io.Fonts->AddFontFromMemoryTTF(KFontTTF, sizeof(KFontTTF), 16.f, &font_cfg);

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