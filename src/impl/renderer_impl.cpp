#include "renderer_impl.h"

#include "flip/camera.h"
#include "imgui/imgui.h"

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

#include "sokol_gfx_utils.h"

// math
#include "hmm/HandmadeMath.h"

namespace flip {

// Global sg_imgui context
static sg_imgui_t g_sg_imgui_ctx;

static SgBuffer mbuf;  // TEMP

bool RendererImpl::Initialize() {
  bool success = true;

  // Setups sokol gfx
  const auto& app_desc = sapp_query_desc();
  sg_setup(sg_desc{.logger = {.func = app_desc.logger.func,
                              .user_data = app_desc.logger.user_data},
                   .context = sapp_sgcontext()});

  // Setups sokol-gl
  sgl_setup(sgl_desc_t{.logger = {.func = app_desc.logger.func,
                                  .user_data = app_desc.logger.user_data}});

  // Setups sokol-gl debug imgui
  const sg_imgui_desc_t desc = {};
  sg_imgui_init(&g_sg_imgui_ctx, &desc);

  return success;
}

RendererImpl::~RendererImpl() {
  mbuf.reset();

  sg_imgui_discard(&g_sg_imgui_ctx);
  sgl_shutdown();
  sg_shutdown();
}

const int kDefaultPassLayer = 0x70501000;
void RendererImpl::BeginDefaultPass(const CameraView& _view) {
  // view-projection matrix...
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
  if (ImGui::BeginMenu("Debug")) {
    ImGui::MenuItem("Buffers", 0, &g_sg_imgui_ctx.buffers.open);
    ImGui::MenuItem("Images", 0, &g_sg_imgui_ctx.images.open);
    ImGui::MenuItem("Samplers", 0, &g_sg_imgui_ctx.samplers.open);
    ImGui::MenuItem("Shaders", 0, &g_sg_imgui_ctx.shaders.open);
    ImGui::MenuItem("Pipelines", 0, &g_sg_imgui_ctx.pipelines.open);
    ImGui::MenuItem("Passes", 0, &g_sg_imgui_ctx.passes.open);
    ImGui::MenuItem("Calls", 0, &g_sg_imgui_ctx.capture.open);
    ImGui::EndMenu();
  }
  sg_imgui_draw(&g_sg_imgui_ctx);

  return true;
}

bool RendererImpl::DrawCubes(std::span<const HMM_Mat4> _transforms) {
  struct S {
    SgShader shd;
    SgPipeline pip;
    SgBuffer vbuf;
    SgBuffer ibuf;
    // SgBuffer mbuf;
    sshape_element_range_t draw;
  } state;

  struct Uniforms {
    HMM_Mat4 vp;
  };

  // Create shader
  auto shader_desc = sg_shader_desc{};
  shader_desc.vs.source =
      "#version 330\n"
      "uniform mat4 vp;\n"
      "layout(location=0) in vec4 position;\n"
      "layout(location=1) in vec3 normal;\n"
      "layout(location=2) in vec2 texcoord;\n"
      "layout(location=3) in vec4 color;\n"
      "layout(location=4) in mat4 model;\n"
      "out vec3 vertex_normal;\n"
      "out vec4 vertex_color;\n"
      "void main() {\n"
      "  gl_Position = vp * model * position;\n"
      "  mat3 cross_matrix = mat3(\n"
      "    cross(model[1].xyz, model[2].xyz),\n"
      "    cross(model[2].xyz, model[0].xyz),\n"
      "    cross(model[0].xyz, model[1].xyz));\n"
      "  float invdet = 1.0 / dot(cross_matrix[2], model[2].xyz);\n"
      "  mat3 normal_matrix = cross_matrix * invdet;\n"
      "  vertex_normal = normal_matrix * normal;\n"
      "  vertex_color = color;\n"
      "}\n";
  shader_desc.vs.uniform_blocks[0] = {
      .size = sizeof(Uniforms),
      .uniforms = {{.name = "vp", .type = SG_UNIFORMTYPE_MAT4}}};
  shader_desc.fs.source =
      "#version 330\n"
      "in vec3 vertex_normal;\n"
      "in vec4 vertex_color;\n"
      "out vec4 frag_color;\n"
      "void main() {\n"
      "  vec3 normal = normalize(vertex_normal);\n"
      "  vec3 alpha = (normal + 1.) * .5;\n"
      "  vec2 bt = mix(vec2(.3, .7), vec2(.4, .8), alpha.xz);\n"
      "  vec3 ambient = mix(vec3(bt.x, .3, bt.x), vec3(bt.y, .8, bt.y), "
      "alpha.y);\n"
      "  frag_color = vec4(ambient, 1.);\n"
      "}\n";
  state.shd = sg_make_shader(shader_desc);

  // Shader and pipeline object
  state.pip = sg_make_pipeline(sg_pipeline_desc{
      .shader = state.shd,
      .layout =
          {.buffers = {sshape_vertex_buffer_layout_state(),
                       {.stride = sizeof(HMM_Mat4),
                        .step_func = SG_VERTEXSTEP_PER_INSTANCE}},
           .attrs = {sshape_position_vertex_attr_state(),
                     sshape_normal_vertex_attr_state(),
                     sshape_texcoord_vertex_attr_state(),
                     sshape_color_vertex_attr_state(),
                     sg_vertex_attr_state{.buffer_index = 1,
                                          .offset = 0,
                                          .format = SG_VERTEXFORMAT_FLOAT4},
                     sg_vertex_attr_state{.buffer_index = 1,
                                          .offset = 16,
                                          .format = SG_VERTEXFORMAT_FLOAT4},
                     sg_vertex_attr_state{.buffer_index = 1,
                                          .offset = 32,
                                          .format = SG_VERTEXFORMAT_FLOAT4},
                     sg_vertex_attr_state{.buffer_index = 1,
                                          .offset = 48,
                                          .format = SG_VERTEXFORMAT_FLOAT4}}},
      .depth = {.compare = SG_COMPAREFUNC_LESS_EQUAL, .write_enabled = true},
      .index_type = SG_INDEXTYPE_UINT16,
      .cull_mode = SG_CULLMODE_BACK,
  });

  // Generate shape geometries
  sshape_vertex_t vertices[24];
  uint16_t indices[36];
  auto buf = sshape_buffer_t{
      .vertices = {.buffer = SSHAPE_RANGE(vertices)},
      .indices = {.buffer = SSHAPE_RANGE(indices)},
  };
  auto box = sshape_box_t{.width = 1.f, .height = 1.f, .depth = 1.f};
  buf = sshape_build_box(&buf, &box);
  state.draw = sshape_element_range(&buf);

  // One vertex/index-buffer-pair for all shapes
  const sg_buffer_desc vbuf_desc = sshape_vertex_buffer_desc(&buf);
  const sg_buffer_desc ibuf_desc = sshape_index_buffer_desc(&buf);
  state.vbuf = sg_make_buffer(&vbuf_desc);
  state.ibuf = sg_make_buffer(&ibuf_desc);

  sg_apply_pipeline(state.pip);

  // Updates instance model matrices
  if (!mbuf || sg_query_buffer_will_overflow(mbuf, _transforms.size_bytes())) {
    mbuf =
        sg_make_buffer(sg_buffer_desc{.size = _transforms.size_bytes(),
                                      .usage = SG_USAGE_STREAM,
                                      .label = "flip:: instance transforms"});
  }
  sg_update_buffer(mbuf, sg_range{.ptr = _transforms.data(),
                                  .size = _transforms.size_bytes()});

  auto bindings = sg_bindings{};
  bindings.vertex_buffers[0] = state.vbuf;
  bindings.vertex_buffers[1] = mbuf;
  bindings.index_buffer = state.ibuf;
  sg_apply_bindings(bindings);

  // per shape model-view-projection matrix
  const auto uniforms = Uniforms{.vp = view_proj_};
  sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(uniforms));
  sg_draw(state.draw.base_element, state.draw.num_elements, _transforms.size());

  return true;
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