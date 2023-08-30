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
#include "imgui.h"
#include "shapes.h"

namespace flip {

using SglPipeline = SgResource<sgl_pipeline, sgl_destroy_pipeline>;

struct ImMode {
  // Z
  bool z_write = true;
  sg_compare_func z_compare = SG_COMPAREFUNC_LESS_EQUAL;

  // Culling
  sg_cull_mode cull_mode = SG_CULLMODE_BACK;

  // Blending
  bool blending = false;
};

bool operator==(ImMode const& _a, ImMode const& _b) noexcept {
  return _a.z_write == _b.z_write && _a.z_compare == _b.z_compare &&
         _a.cull_mode == _b.cull_mode && _a.blending == _b.blending;
}

struct ImModeHash {
  std::size_t operator()(ImMode const& _mode) const noexcept {
    auto hash =
        std::size_t(_mode.z_write) << 0 | std::size_t(_mode.z_compare) << 1 |
        std::size_t(_mode.cull_mode) << 10 | std::size_t(_mode.blending) << 12;
    return hash;
  }
};

// Imdraw pipelines
std::unordered_map<ImMode, SglPipeline, ImModeHash> pipelines;

class ImDrawer {
 public:
  ImDrawer(Renderer& _renderer, const HMM_Mat4& _transform = HMM_M4D(1),
           const ImMode& _mode = {}) {
    auto it = pipelines.find(_mode);
    if (it == pipelines.end()) {
      sgl_pipeline pip = sgl_make_pipeline(sg_pipeline_desc{
          .cull_mode = _mode.cull_mode,
          .colors = {{.blend = {.enabled = _mode.blending,
                                .src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA,
                                .dst_factor_rgb =
                                    SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA

                      }}},
          .depth =
              {
                  .write_enabled = _mode.z_write,
                  .compare = _mode.z_compare,
              },
          .label = "flip: ImDrawer"});
      it = pipelines.emplace(_mode, pip).first;
    }

    sgl_defaults();

    sgl_push_pipeline();
    sgl_load_pipeline(it->second);

    sgl_matrix_mode_projection();
    sgl_load_matrix(_renderer.GetViewProj().Elements[0]);

    sgl_matrix_mode_modelview();
    sgl_load_matrix(_transform.Elements[0]);
  }
  ~ImDrawer() {
    sgl_pop_pipeline();

    auto error = sgl_error();
    (void)error;
    assert(error == SGL_NO_ERROR &&
           "A sgl error was triggered during ImDrawer scope");
  }
};

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
  pipelines.clear();  // temp

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

  imgui_->BeginFrame();
}

void RendererImpl::EndDefaultPass() {
  auto error = sgl_error();
  (void)error;
  assert(error == SGL_NO_ERROR &&
         "A sgl error was triggered during pass scope");
  sgl_draw();

  imgui_->EndFrame();

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

bool RendererImpl::DrawShapes(std::span<const HMM_Mat4> _transforms,
                              Shape _shape) {
  // Updates model space matrices buffer
  auto buffer_binding = resources_->transforms_buffer.Append(
      std::as_bytes(std::span{_transforms}));

  // Draw
  return resources_->shapes.Draw(_shape, _transforms.size(), buffer_binding,
                                 view_proj_);
}

bool RendererImpl::DrawAxes(std::span<const HMM_Mat4> _transforms) {
  auto drawer = ImDrawer{*this};
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
    auto drawer = ImDrawer{
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
    auto drawer = ImDrawer{*this};
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