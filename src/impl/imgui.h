#pragma once

#include "flip/utils/sokol_gfx.h"

struct sapp_event;

namespace flip {
// Base Renderer interface
class Imgui {
 public:
  virtual ~Imgui();

  virtual bool Initialize();
  virtual bool Event(const sapp_event& _event);
  virtual bool Menu() { return true; }

  virtual void BeginFrame();
  virtual void EndFrame();

 protected:
 private:
  SgImage font_image_;
  SgSampler font_sampler_;
};

}  // namespace flip