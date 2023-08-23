#pragma once
#include <memory>

#include "flip/renderer.h"

// math
#include "hmm/HandmadeMath.h"

namespace flip {
struct CameraView;

// Base Renderer interface
class RendererImpl : public Renderer {
 public:
  RendererImpl();
  virtual ~RendererImpl();

 protected:
  virtual bool Initialize() override;
  virtual bool Menu() override;

  virtual bool DrawShape(Shape _shape,
                         std::span<const HMM_Mat4> _transforms) override;
  virtual bool DrawAxes(std::span<const HMM_Mat4> _transforms) override;

  virtual const HMM_Mat4& GetViewProj() const override { return view_proj_; }

 private:
  virtual void BeginDefaultPass(const CameraView& _view) override;
  virtual void EndDefaultPass() override;

  // Declares a resource container:
  // - Prevents from including sokol here and messing the header.
  // - Releases all resources at once
  struct Resources;
  std::unique_ptr<Resources> resources_;

  // View projection matrix
  HMM_Mat4 view_proj_;
};

}  // namespace flip