#pragma once

#include "flip/renderer.h"

// math
#include "hmm/HandmadeMath.h"

namespace flip {
struct CameraView;

// Base Renderer interface
class RendererImpl : public Renderer {
 public:
  RendererImpl() = default;
  virtual ~RendererImpl();

  virtual bool Initialize() override;
  virtual bool Menu() override;

  virtual const HMM_Mat4& GetViewProj() const override { return view_proj_; }

  virtual bool DrawCubes(std::span<const HMM_Mat4> _transforms) override;
  virtual bool DrawAxes(std::span<const HMM_Mat4> _transforms) override;

 protected:
 private:
  virtual void BeginDefaultPass(const CameraView& _view) override;
  virtual void EndDefaultPass() override;

  HMM_Mat4 view_proj_;  // Temp to avoid including math
};

}  // namespace flip