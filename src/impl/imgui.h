#pragma once

#include "flip/utils/sokol_gfx.h"

struct sapp_event;

namespace flip {
// Base Renderer interface
class Imgui {
 public:
  Imgui();
  virtual ~Imgui();

  bool Event(const sapp_event& _event);
  bool Menu() { return true; }

  void BeginFrame();
  void EndFrame();

 protected:
 private:
  SgImage font_image_;
  SgSampler font_sampler_;
};

}  // namespace flip