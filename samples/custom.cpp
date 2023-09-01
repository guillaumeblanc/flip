#include "flip/application.h"
#include "flip/renderer.h"
#include "flip/utils/sokol_gfx.h"
#include "hmm/HandmadeMath.h"

// Implement the custom flip::Application.
class Custom : public flip::Application {
 public:
  Custom() : flip::Application(Settings{.title = "Custom"}) {}

  virtual bool Initialize(bool _headless) override {
    if (_headless) {
      return true;
    }
    // A vertex buffer
    const float vertices[] = {// positions       // color
                              0.f,   10.f,  0.f, 0.f, 1.f, 0.f, 1.f,
                              10.f,  -10.f, 0.f, 1.f, 0.f, 0.f, 1.f,
                              -10.f, -10.f, 0.f, 0.f, 0.f, 1.f, 1.f};
    vertex_buffer_ =
        flip::MakeSgBuffer(sg_buffer_desc{.data = SG_RANGE(vertices)});

    // A shader
    auto shader_desc = sg_shader_desc{};
    shader_desc.vs.uniform_blocks[0] = {
        .size = sizeof(HMM_Mat4),
        .uniforms = {{.name = "mvp", .type = SG_UNIFORMTYPE_MAT4}}};
    shader_desc.vs.source =
        "#version 330\n"
        "uniform mat4 mvp;\n"
        "layout(location=0) in vec3 position;\n"
        "layout(location=1) in vec4 color;\n"
        "out vec4 vertex_color;\n"
        "void main() {\n"
        "  gl_Position = mvp * vec4(position, 1.);\n"
        "  vertex_color = color;\n"
        "}\n";
    shader_desc.fs.source =
        "#version 330\n"
        "in vec4 vertex_color;\n"
        "out vec4 frag_color;\n"
        "void main() {\n"
        "  frag_color = vertex_color;\n"
        "}\n";
    shader_ = flip::MakeSgShader(shader_desc);

    // A minimal pipeline state object
    pipeline_ = flip::MakeSgPipeline(sg_pipeline_desc{
        .shader = shader_,
        .layout = {.buffers = {{.stride = 28}},
                   .attrs = {{.format = SG_VERTEXFORMAT_FLOAT3},
                             {.format = SG_VERTEXFORMAT_FLOAT4}}},
        .depth = {.compare = SG_COMPAREFUNC_LESS_EQUAL, .write_enabled = true},
        .cull_mode = SG_CULLMODE_BACK,
    });
    return true;
  }

  virtual bool Display(flip::Renderer& _renderer, float _time, float _dt,
                       float _inv_dt) override {
    sg_apply_pipeline(pipeline_);

    auto bind = sg_bindings{.vertex_buffers = {vertex_buffer_}};
    sg_apply_bindings(&bind);
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 0,
                      {_renderer.GetViewProj().Elements[0], 64});
    sg_draw(0, 3, 1);

    return true;
  }

 private:
  flip::SgBuffer vertex_buffer_;
  flip::SgShader shader_;
  flip::SgPipeline pipeline_;
};

// Application instantiation function
std::unique_ptr<flip::Application> InstantiateApplication() {
  return std::make_unique<Custom>();
}
