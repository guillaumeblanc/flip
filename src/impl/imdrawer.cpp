#include "imdrawer.h"

#include <cassert>

#include "sokol/sokol_app.h"

namespace flip {

ImDrawer::ImDrawer() {
  // Setups sokol-gl
  const auto& app_desc = sapp_query_desc();
  sgl_setup(sgl_desc_t{.logger = {.func = app_desc.logger.func,
                                  .user_data = app_desc.logger.user_data}});
}

ImDrawer::~ImDrawer() {
  pipelines_.clear();
  sgl_shutdown();
}

void ImDrawer::Begin(const HMM_Mat4& _view_proj, const HMM_Mat4& _transform,
                     const ImMode& _mode) {
  auto it = pipelines_.find(_mode);
  if (it == pipelines_.end()) {
    sgl_pipeline pip = sgl_make_pipeline(sg_pipeline_desc{
        .depth = {.compare = _mode.z_compare, .write_enabled = _mode.z_write},
        .colors = {{.blend = {.enabled = _mode.blending,
                              .src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA,
                              .dst_factor_rgb =
                                  SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA

                    }}},
        .cull_mode = _mode.cull_mode,
        .label = "flip: ImDrawer"});
    it = pipelines_.emplace(_mode, pip).first;
  }

  sgl_defaults();

  sgl_push_pipeline();
  sgl_load_pipeline(it->second);

  sgl_matrix_mode_projection();
  sgl_load_matrix(_view_proj.Elements[0]);

  sgl_matrix_mode_modelview();
  sgl_load_matrix(_transform.Elements[0]);
}

void ImDrawer::End() {
  sgl_pop_pipeline();

  auto error = sgl_error();
  (void)error;
  assert(error == SGL_NO_ERROR &&
         "A sgl error was triggered during ImDrawer scope");
}

}  // namespace flip