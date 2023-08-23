#pragma once
#include <span>

union HMM_Mat4;

namespace flip {
struct CameraView;

// Helper functions to construct a span from a single object.
template <typename _T>
auto make_span(const _T& _t) {
  return std::span<const _T>{&_t, 1};
}
template <typename _T>
auto make_span(_T& _t) {
  return std::span<_T>{&_t, 1};
}

// Defines render Color structure.
struct Color {
  float r, g, b, a;
};

// Color constants.
static const Color kRed = {1, 0, 0, 1};
static const Color kGreen = {0, 1, 0, 1};
static const Color kBlue = {0, 0, 1, 1};
static const Color kWhite = {1, 1, 1, 1};
static const Color kYellow = {1, 1, 0, 1};
static const Color kMagenta = {1, 0, 1, 1};
static const Color kCyan = {0, 1, 1, 1};
static const Color kGrey = {.5f, .5f, .5f, 1};
static const Color kBlack = {.5f, .5f, .5f, 1};

// Base Renderer interface
class Renderer {
 public:
  virtual ~Renderer() = default;

  virtual bool Initialize() { return true; }
  virtual bool Menu() { return true; }

  // RAII to begin/end the default rendering pass
  class DefaultPass {
   public:
    DefaultPass(Renderer& _renderer, const CameraView& _view)
        : renderer_(_renderer) {
      renderer_.BeginDefaultPass(_view);
    }
    ~DefaultPass() { renderer_.EndDefaultPass(); }

   private:
    Renderer& renderer_;
  };

  virtual const HMM_Mat4& GetViewProj() const = 0;

  enum Shape {
    kPlane,     // Size of (1, 0, 1), with origin in the corner (0, 0, 0).
    kCube,      // Size of (1, 1, 1) with origin in the corner (0, 0, 0).
    kSphere,    // Radius of .5, with origin at sphere center (.5, .5, .5)
    kCylinder,  // Radius of .5, with origin at center of bottom disk
    kTorus      // Radius of .4, ring radius of .1, with origin at torus center
  };
  virtual bool DrawShape(Shape _shape,
                         std::span<const HMM_Mat4> _transforms) = 0;
  virtual bool DrawAxes(std::span<const HMM_Mat4> _transforms) = 0;

 protected:
 private:
  virtual void BeginDefaultPass(const CameraView& _view) = 0;
  virtual void EndDefaultPass() = 0;
};

}  // namespace flip