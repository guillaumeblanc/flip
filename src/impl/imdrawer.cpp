#include "imdrawer.h"

#include <cassert>

#include "sokol/sokol_app.h"

namespace flip {

ImDrawer::ImDrawer() {
  // Shader
  auto shader_desc = sg_shader_desc{.label = "flip:: ImDrawer"};
  shader_desc.vs.uniform_blocks[0] = {
      .size = sizeof(HMM_Mat4),
      .uniforms = {{.name = "mvp", .type = SG_UNIFORMTYPE_MAT4}}};
  shader_desc.vs.source =
      "#version 330\n"
      "uniform mat4 mvp;\n"
      "layout(location=0) in vec3 position;\n"
      "layout(location=1) in vec4 color;\n"
      "layout(location=2) in vec2 uv;\n"
      "out vec2 vertex_uv;\n"
      "out vec4 vertex_color;\n"
      "void main() {\n"
      "  gl_Position = mvp * vec4(position, 1.);\n"
      "  vertex_uv = uv;\n"
      "  vertex_color = color;\n"
      "}\n";
  shader_desc.fs.images[0] = {.used = true};
  shader_desc.fs.samplers[0] = {.used = true};
  shader_desc.fs.image_sampler_pairs[0] = {
      .used = true, .image_slot = 0, .sampler_slot = 0, .glsl_name = "tex"};
  shader_desc.fs.source =
      "#version 330\n"
      "uniform sampler2D tex;\n"
      "in vec2 vertex_uv;\n"
      "in vec4 vertex_color;\n"
      "out vec4 frag_color;\n"
      "void main() {\n"
      "  frag_color = texture(tex, vertex_uv) * vertex_color;\n"
      "  if(frag_color.a <= 0.) discard;\n"
      "}\n";
  shader_ = flip::MakeSgShader(shader_desc);

  // Image
  uint32_t pixels[] = {0xFFFFFFFF};  // Single white pixel
  image_ = flip::MakeSgImage(
      sg_image_desc{.width = 1,
                    .height = 1,
                    .data = {.subimage = {{SG_RANGE(pixels)}}},
                    .label = "flip: ImDrawer default"});

  // Sampler
  sampler_ =
      flip::MakeSgSampler(sg_sampler_desc{.min_filter = SG_FILTER_LINEAR,
                                          .mag_filter = SG_FILTER_LINEAR,
                                          .label = "flip: ImDrawer default"});
}

ImDrawer::~ImDrawer() {}

void ImDrawer::Begin(const HMM_Mat4& _view_proj, const HMM_Mat4& _transform,
                     const ImMode& _mode) {
  auto it = pipelines_.find(_mode);
  if (it == pipelines_.end()) {
    sg_pipeline pip = sg_make_pipeline(sg_pipeline_desc{
        .shader = shader_,
        .layout = {.buffers = {{.stride = 36}},
                   .attrs = {{.format = SG_VERTEXFORMAT_FLOAT3},
                             {.format = SG_VERTEXFORMAT_FLOAT4},
                             {.format = SG_VERTEXFORMAT_FLOAT2}}},
        .depth = {.compare = _mode.z_compare, .write_enabled = _mode.z_write},
        .colors = {{.blend = {.enabled = _mode.blending,
                              .src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA,
                              .dst_factor_rgb =
                                  SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA

                    }}},
        .primitive_type = _mode.type,
        .cull_mode = _mode.cull_mode,
        .label = "flip: ImDrawer"});
    it = pipelines_.emplace(_mode, pip).first;
  }

  sg_apply_pipeline(it->second);

  const auto mvp = _view_proj * _transform;
  sg_apply_uniforms(SG_SHADERSTAGE_VS, 0, {mvp.Elements[0], 64});
}

void ImDrawer::End(std::span<const ImVertex> _vertices, sg_image _image,
                   sg_sampler _sampler) {
  // Updates vertices buffer
  auto buffer_binding = buffer_.Append(std::as_bytes(_vertices));

  sg_apply_bindings(sg_bindings{
      .vertex_buffers = {buffer_binding.id},
      .vertex_buffer_offsets = {buffer_binding.offset},
      .fs = {
          .images = {_image.id != SG_INVALID_ID ? _image : image_},
          .samplers = {_sampler.id != SG_INVALID_ID ? _sampler : sampler_}}});
  sg_draw(0, _vertices.size(), 1);
}

}  // namespace flip