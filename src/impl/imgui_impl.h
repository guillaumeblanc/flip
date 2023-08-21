#pragma once

#include "flip/imgui.h"
#include "sokol_gfx_utils.h"

namespace flip {

// Base Renderer interface
class ImguiImpl : public Imgui {
 public:
  virtual ~ImguiImpl();

  virtual bool Initialize() override;
  virtual bool Event(const sapp_event& _event) override;
  virtual bool Menu() override { return true; }

 protected:
 private:
  virtual void BeginFrame() override;
  virtual void EndFrame() override;

  virtual void BeginMainMenu() override;
  virtual void EndMainMenu() override;

  SgImage font_image_;
  SgSampler font_sampler_;
};

}  // namespace flip