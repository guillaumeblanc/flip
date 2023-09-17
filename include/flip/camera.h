#pragma once

// math
#include "flip/math.h"

struct sapp_event;

namespace flip {

// Camera view parameters, passed to the renderer.
struct CameraView {
  float fov;
  HMM_Vec3 center;
  HMM_Vec3 eye;
};

// Base Camera interface
class Camera {
 public:
  virtual ~Camera() = default;

  virtual bool Update(float _time, float _dt, float _inv_dt) { return true; }
  virtual bool Event(const sapp_event& _event) { return false; }
  virtual bool Menu() { return true; }

  virtual const CameraView& GetCameraView() const = 0;

 protected:
 private:
};

}  // namespace flip