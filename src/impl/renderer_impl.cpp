#include "renderer_impl.h"

#include <unordered_map>  // temp imdraw

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

// flip interfaces
#include "flip/camera.h"
#include "flip/utils/sokol_gfx.h"

// flip implementations
#include "factory.h"
#include "imdrawer.h"
#include "imgui.h"
#include "shapes.h"

namespace flip {

struct RendererImpl::Resources {
  // Warning: order of members matters for construction / destruction order

  // flip imgui
  Imgui imgui;

  // Context for debug-inspection UI for sokol_gfx.h
  struct SgImgui {
    SgImgui() {
      // Setups sokol-gl debug imgui
      const sg_imgui_desc_t desc = {};
      sg_imgui_init(&context, &desc);
    }

    ~SgImgui() { sg_imgui_discard(&context); }
    sg_imgui_t context;
  } sg_imgui;

  // flip imdrawer
  ImDrawer im_drawer;

  // Buffer of transforms used for instanced rendering.
  SgDynamicBuffer transforms_buffer;

  // Primitive shapes
  Shapes shapes;
};

RendererImpl::RendererImpl() {}

bool RendererImpl::Initialize() {
  bool success = true;

  // Setups sokol gfx
  const auto& app_desc = sapp_query_desc();
  sg_setup(sg_desc{.logger = {.func = app_desc.logger.func,
                              .user_data = app_desc.logger.user_data},
                   .context = sapp_sgcontext()});

  // Allocates the resource container once gfx is ready
  resources_ = std::make_unique<Resources>();

  // Initialize shape resources
  success &= resources_->shapes.Initialize();

  return success;
}

RendererImpl::~RendererImpl() {
  // Deallocates all resources.
  resources_ = nullptr;

  sg_shutdown();
}

bool RendererImpl::Event(const sapp_event& _event) {
  return resources_->imgui.Event(_event);
}

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

  resources_->imgui.BeginFrame();
}

void RendererImpl::EndDefaultPass() {
  auto error = sgl_error();
  (void)error;
  assert(error == SGL_NO_ERROR &&
         "A sgl error was triggered during pass scope");
  sgl_draw();

  resources_->imgui.EndFrame();

  sg_end_pass();
  sg_commit();
}

void RendererImpl::BeginImDraw(const HMM_Mat4& _transform,
                               const ImMode& _mode) {
  resources_->im_drawer.Begin(view_proj_, _transform, _mode);
}
void RendererImpl::EndImDraw() { resources_->im_drawer.End(); }

bool RendererImpl::Menu() {
  // Sokol gl debug menu
  auto& ctx = resources_->sg_imgui.context;
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

bool RendererImpl::DrawShapes(std::span<const HMM_Mat4> _transforms,
                              Shape _shape, Color _color) {
  // Updates model space matrices buffer
  auto buffer_binding = resources_->transforms_buffer.Append(
      std::as_bytes(std::span{_transforms}));

  // Draw
  return resources_->shapes.Draw(_shape, _color, _transforms.size(),
                                 buffer_binding, view_proj_);
}

bool RendererImpl::DrawAxes(std::span<const HMM_Mat4> _transforms) {
  auto drawer = ImDraw{*this, HMM_M4D(1), {}};
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

bool RendererImpl::DrawGrids(std::span<const HMM_Mat4> _transforms,
                             int _cells) {
  const float kCellSize = 1.f;
  const float extent = _cells * kCellSize;
  const auto corner = HMM_Vec3{-extent, 0, -extent} * .5f;

  // Alpha blended surface
  {
    auto drawer = ImDraw{
        *this, HMM_M4D(1), {.cull_mode = SG_CULLMODE_NONE, .blending = true}};
    for (auto& transform : _transforms) {
      sgl_load_matrix(transform.Elements[0]);

      sgl_begin_triangle_strip();
      sgl_c4b(0x80, 0xc0, 0xd0, 0xb0);
      sgl_v3f(corner.X, corner.Y, corner.Z);
      sgl_v3f(corner.X, corner.Y, corner.Z + extent);
      sgl_v3f(corner.X + extent, corner.Y, corner.Z);
      sgl_v3f(corner.X + extent, corner.Y, corner.Z + extent);
      sgl_end();
    }
  }

  // Opaque grid lines
  {
    auto drawer = ImDraw{*this, HMM_M4D(1), ImMode{}};
    for (auto& transform : _transforms) {
      sgl_load_matrix(transform.Elements[0]);

      sgl_begin_lines();
      sgl_c4b(0x54, 0x55, 0x50, 0xff);

      // Renders lines along X axis.
      auto begin = corner, end = corner;
      end.X += extent;
      for (int i = 0; i < _cells + 1; ++i) {
        sgl_v3f(begin.X, begin.Y, begin.Z);
        sgl_v3f(end.X, end.Y, end.Z);
        begin.Z += kCellSize;
        end.Z += kCellSize;
      }
      // Renders lines along Z axis.
      begin = end = corner;
      end.Z += extent;
      for (int i = 0; i < _cells + 1; ++i) {
        sgl_v3f(begin.X, begin.Y, begin.Z);
        sgl_v3f(end.X, end.Y, end.Z);
        begin.X += kCellSize;
        end.X += kCellSize;
      }
      sgl_end();
    }
  }
  return true;
}
}  // namespace flip