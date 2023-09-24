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
  bool Orbit(const HMM_Vec2& _delta);
  bool Zoom(float _d);
  bool Pan(const HMM_Vec2& _delta);
  bool Move(const HMM_Vec2& _delta);

  HMM_Vec2 last_touches_[SAPP_MAX_TOUCHPOINTS];

  const float kMinDist = 1.f;
  const float kMaxDist = 100.f;
  const float kMinLat = -kPi_2 * .95f;
  const float kMaxLat = kPi_2 * .95f;
  const float kOrbitFactor = .01f;
  const float kOrbitTouchFactor = .001f;
  const float kPanFactor = .002f;
  const float kPanTouchFactor = .0004f;
  const float kMoveFactor = .001f;
  const float kZoomFactor = .2f;
  const float kZoomTouchFactor = .005f;

  float distance_ = 7.f;
  float latitude_ = 0.f;
  float longitude_ = 0.f;
  CameraView camera_view_ = {.fov = kPi_2, .center = {0, 6, 0}};
};

}  // namespace flip