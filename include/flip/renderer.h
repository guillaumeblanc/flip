#pragma once
#include <span>

union HMM_Mat4;
struct sapp_event;

// TEMP
struct sg_image;
struct sg_sampler;

namespace flip {
struct CameraView;
struct ImMode;
struct ImVertex;

union Color {
  struct {
    float r, g, b, a;
  };
  float rgba[4];
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
static const Color kBlack = {0, 0, 0, 1};

// Base Renderer interface
class Renderer {
 public:
  virtual ~Renderer() = default;

  virtual bool Initialize() { return true; }
  virtual bool Event(const sapp_event& _event) { return false; }
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

  // Renders shapes, as described by Shape enumeration
  enum Shape {
    kPlane,     // Size of (1, 0, 1), with origin at plane center (.5, 0, .5).
    kCube,      // Size of (1, 1, 1) with origin in the box center (.5, .5, .5).
    kSphere,    // Radius of .5, with origin at sphere center (.5, .5, .5)
    kCylinder,  // Radius of .5, with origin at center of bottom disk
    kTorus,     // Radius of .4, ring radius of .1, with origin at torus center
    kCount
  };
  virtual bool DrawShapes(std::span<const HMM_Mat4> _transforms, Shape _shape,
                          Color _color) = 0;

  // Renders xyz coordinate system.
  virtual bool DrawAxes(std::span<const HMM_Mat4> _transforms) = 0;

  // Renders yz grids of _cells, with origin at plane center.
  virtual bool DrawGrids(std::span<const HMM_Mat4> _transforms, int _cells) = 0;

 protected:
 private:
  virtual void BeginDefaultPass(const CameraView& _view) = 0;
  virtual void EndDefaultPass() = 0;

  friend class ImDraw;
  virtual void BeginImDraw(const HMM_Mat4& _transform, const ImMode& _mode) = 0;
  virtual void EndImDraw(std::span<const ImVertex> vertices_, sg_image _image,
                         sg_sampler _sampler) = 0;
};

}  // namespace flip