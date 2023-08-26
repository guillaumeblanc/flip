#include "renderer_impl.h"

// Sokol library, do not sort includes
// clang-format off
#include "sokol/sokol_app.h"
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_glue.h"
#include "sokol/util/sokol_gl.h"
#include "sokol/util/sokol_imgui.h"
#include "sokol/util/sokol_gfx_imgui.h"
#include "sokol/util/sokol_shape.h"
// clang-format on

// Dear ImGui
#include "imgui/imgui.h"

// math
#include "hmm/HandmadeMath.h"

// flip
#include "factory.h"
#include "flip/camera.h"
#include "flip/imgui.h"
#include "flip/utils/sokol_gfx.h"
#include "shapes.h"

namespace flip {

struct RendererImpl::Resources {
  // Context for debug-inspection UI for sokol_gfx.h
  sg_imgui_t sg_imgui_ctx;

  // Buffer of transforms used for instanced rendering.
  SgDynamicBuffer transforms_buffer;

  // Primitive shapes
  Shapes shapes;
};

RendererImpl::RendererImpl() : imgui_(Factory().InstantiateImgui()) {}

bool RendererImpl::Initialize() {
  bool success = true;

  // Allocates the resource container
  resources_ = std::make_unique<Resources>();

  // Setups sokol gfx
  const auto& app_desc = sapp_query_desc();
  sg_setup(sg_desc{.logger = {.func = app_desc.logger.func,
                              .user_data = app_desc.logger.user_data},
                   .context = sapp_sgcontext()});

  // Setups sokol-gl
  sgl_setup(sgl_desc_t{.logger = {.func = app_desc.logger.func,
                                  .user_data = app_desc.logger.user_data}});

  // Setup imgui
  success &= imgui_->Initialize();

  // Setups sokol-gl debug imgui
  const sg_imgui_desc_t desc = {};
  sg_imgui_init(&resources_->sg_imgui_ctx, &desc);

  // Initialize shape resources
  success &= resources_->shapes.Initialize();

  return success;
}

RendererImpl::~RendererImpl() {
  // Deallocates all resources.
  sg_imgui_discard(&resources_->sg_imgui_ctx);
  imgui_ = nullptr;
  resources_ = nullptr;

  sgl_shutdown();
  sg_shutdown();
}

bool RendererImpl::Event(const sapp_event& _event) {
  return imgui_->Event(_event);
}

const int kDefaultPassLayer = 0x70501000;
void RendererImpl::BeginDefaultPass(const CameraView& _view) {
  // Builds view-projection matrix...
  HMM_Mat4 proj = HMM_Perspective_RH_ZO(
      _view.fov, sapp_widthf() / sapp_heightf(), 0.01f, 100.0f);
  HMM_Mat4 view =
      HMM_LookAt_RH(_view.eye, _view.center, HMM_Vec3{0.0f, 1.0f, 0.0f});
  view_proj_ = proj * view;

  const auto action =
      sg_pass_action{.colors = {{.load_action = SG_LOADACTION_CLEAR,
                                 .store_action = SG_STOREACTION_STORE,
                                 .clear_value = {.1f, .1f, .1f, 1.f}}}};

  sg_begin_default_pass(&action, sapp_width(), sapp_height());
  sgl_layer(kDefaultPassLayer);
}

void RendererImpl::EndDefaultPass() {
  sgl_draw_layer(kDefaultPassLayer);
  sg_end_pass();
  sg_commit();
}

bool RendererImpl::Menu() {
  // Sokol gl debug menu
  auto& ctx = resources_->sg_imgui_ctx;
  if (ImGui::BeginMenu("Debug")) {
    ImGui::MenuItem("Buffers", 0, &ctx.buffers.open);
    ImGui::MenuItem("Images", 0, &ctx.images.open);
    ImGui::MenuItem("Samplers", 0, &ctx.samplers.open);
    ImGui::MenuItem("Shaders", 0, &ctx.shaders.open);
    ImGui::MenuItem("Pipelines", 0, &ctx.pipelines.open);
    ImGui::MenuItem("Passes", 0, &ctx.passes.open);
    ImGui::MenuItem("Calls", 0, &ctx.capture.open);
    ImGui::EndMenu();
  }
  sg_imgui_draw(&ctx);

  return true;
}

bool RendererImpl::DrawShape(Shape _shape,
                             std::span<const HMM_Mat4> _transforms) {
  // Updates instance model matrices buffer
  auto buffer_binding = resources_->transforms_buffer.Append(
      std::as_bytes(std::span{_transforms}));

  // Draw
  return resources_->shapes.Draw(_shape, _transforms.size(), buffer_binding,
                                 view_proj_);
}

bool RendererImpl::DrawAxes(std::span<const HMM_Mat4> _transforms) {
  // Layers im rendering, so only calls registered within this pass are
  // rendered.

  sgl_defaults();
  sgl_matrix_mode_projection();
  sgl_load_matrix(view_proj_.Elements[0]);
  sgl_matrix_mode_modelview();
  for (auto& transform : _transforms) {
    sgl_load_matrix(transform.Elements[0]);

    sgl_begin_lines();
    sgl_c4b(0xff, 0, 0, 0xff);
    sgl_v3f(0, 0, 0);
    sgl_v3f(1, 0, 0);

    sgl_c4b(0, 0xff, 0, 0xff);
    sgl_v3f(0, 0, 0);
    sgl_v3f(0, 1, 0);

    sgl_c4b(0, 0, 0xff, 0xff);
    sgl_v3f(0, 0, 0);
    sgl_v3f(0, 0, 1);
    sgl_end();
  }

  return true;
}
}  // namespace flip