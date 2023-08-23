#include "shapes.h"

#include <cassert>

// Sokol library, do not sort includes
// clang-format off
#include "sokol/util/sokol_shape.h"
// clang-format on

#include "hmm/HandmadeMath.h"

namespace flip {

struct Uniforms {
  HMM_Mat4 vp;
};
bool Shapes::Initialize() {
  bool success = true;

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
  shader_ = MakeSgShader(shader_desc);

  // Shader and pipeline object
  pipeline_ = MakeSgPipeline(sg_pipeline_desc{
      .shader = shader_,
      .layout = {.buffers = {sshape_vertex_buffer_layout_state(),
                             {.stride = sizeof(HMM_Mat4),
                              .step_func = SG_VERTEXSTEP_PER_INSTANCE}},
                 .attrs =
                     {
                         sshape_position_vertex_attr_state(),
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
                                              .format = SG_VERTEXFORMAT_FLOAT4},
                     }},
      .depth = {.compare = SG_COMPAREFUNC_LESS_EQUAL, .write_enabled = true},
      .index_type = SG_INDEXTYPE_UINT16,
      .cull_mode = SG_CULLMODE_BACK,
  });

  // Generate shape geometries
  sshape_vertex_t vertices[6 * 1024];
  uint16_t indices[16 * 1024];
  auto buf = sshape_buffer_t{
      .vertices = {.buffer = SSHAPE_RANGE(vertices)},
      .indices = {.buffer = SSHAPE_RANGE(indices)},
  };
  const auto box = sshape_box_t{.width = 1.f,
                                .height = 1.f,
                                .depth = 1.f,
                                .tiles = 1,
                                .transform = {.m = {{1.f, 0.f, 0.f, 0.f},
                                                    {0.f, 1.f, 0.f, 0.f},
                                                    {0.f, 0.f, 1.f, 0.f},
                                                    {.5f, .5f, .5f, 1.f}}}};
  auto to_range =
      [](const sshape_element_range_t& _sdraw) -> std::pair<int, int> {
    return {_sdraw.base_element, _sdraw.num_elements};
  };
  buf = sshape_build_box(&buf, &box);
  draws_[Renderer::Shape::kCube] = to_range(sshape_element_range(&buf));

  const auto plane = sshape_plane_t{.width = 1.f,
                                    .depth = 1.f,
                                    .tiles = 1,
                                    .transform = {.m = {{1.f, 0.f, 0.f, 0.f},
                                                        {0.f, 1.f, 0.f, 0.f},
                                                        {0.f, 0.f, 1.f, 0.f},
                                                        {.5f, 0.f, .5f, 1.f}}}};
  buf = sshape_build_plane(&buf, &plane);
  draws_[Renderer::Shape::kPlane] = to_range(sshape_element_range(&buf));

  const auto sphere = sshape_sphere_t{
      .radius = .5f,
      .slices = 36,
      .stacks = 20,
  };
  buf = sshape_build_sphere(&buf, &sphere);
  draws_[Renderer::Shape::kSphere] = to_range(sshape_element_range(&buf));

  const auto cylinder = sshape_cylinder_t{
      .radius = .5f,
      .height = 1.f,
      .slices = 36,
      .stacks = 1,
  };
  buf = sshape_build_cylinder(&buf, &cylinder);
  draws_[Renderer::Shape::kCylinder] = to_range(sshape_element_range(&buf));

  const auto torus = sshape_torus_t{
      .radius = .4f,
      .ring_radius = .1f,
      .sides = 18,
      .rings = 36,
  };
  buf = sshape_build_torus(&buf, &torus);
  draws_[Renderer::Shape::kTorus] = to_range(sshape_element_range(&buf));
  assert(buf.valid);

  const sg_buffer_desc vbuf_desc = sshape_vertex_buffer_desc(&buf);
  const sg_buffer_desc ibuf_desc = sshape_index_buffer_desc(&buf);
  vertex_buffer_ = MakeSgBuffer(vbuf_desc);
  index_buffer_ = MakeSgBuffer(ibuf_desc);

  return success;
}

bool Shapes::Draw(flip::Renderer::Shape _shape, int _intances,
                  const sg_buffer& transforms, HMM_Mat4& _view_proj) {
  sg_apply_pipeline(pipeline_);

  auto bindings = sg_bindings{};
  bindings.vertex_buffers[0] = vertex_buffer_;
  bindings.vertex_buffers[1] = transforms;
  bindings.index_buffer = index_buffer_;
  sg_apply_bindings(bindings);

  // per shape model-view-projection matrix
  const auto uniforms = Uniforms{.vp = _view_proj};
  sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, SG_RANGE(uniforms));
  sg_draw(draws_[_shape].first, draws_[_shape].second, _intances);
  return true;
}

}  // namespace flip