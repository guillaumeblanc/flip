#pragma once

#include "flip/camera.h"
#include "sokol/sokol_app.h"

namespace flip {

// Base Renderer interface
class OrbitCamera : public Camera {
 public:
  virtual ~OrbitCamera() = default;

 protected:
  virtual bool Update(float _time, float _dt, float _inv_dt) override;
  virtual bool Event(const sapp_event& _event) override;
  virtual bool Menu() override;

  virtual const CameraView& GetCameraView() const override {
    return camera_view_;
  }

 private:
  bool Orbit(float _dx, float _dy);
  bool Zoom(float _d);
  bool Pan(float _dx, float _dy);
  bool Move(float _dx, float _dy);

  HMM_Vec2 last_touches_[SAPP_MAX_TOUCHPOINTS];

  const float kMinDist = 1.f;
  const float kMaxDist = 100.f;
  const float kMinLat = -kPi_2 * .95f;
  const float kMaxLat = kPi_2 * .95f;
  const float kOrbitFactor = .01f;
  const float kOrbitTouchFactor = .3f;
  const float kPanFactor = .002f;
  const float kMoveFactor = .002f;
  const float kZoomFactor = .5f;
  const float kZoomTouchFactor = .02f;

  float distance_ = 7.f;
  float latitude_ = 0.f;
  float longitude_ = 0.f;
  CameraView camera_view_ = {.fov = kPi_2, .center = {0, 6, 0}};
};

}  // namespace flip