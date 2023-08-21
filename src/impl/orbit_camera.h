#pragma once

#include "flip/camera.h"

namespace flip {

// Base Renderer interface
class OrbitCamera : public Camera {
 public:
  virtual ~OrbitCamera() = default;

  virtual bool Update(float _time, float _dt, float _inv_dt) override;
  virtual bool Event(const sapp_event& _event) override;
  virtual bool Menu() override;

  virtual const CameraView& GetCameraView() const override {
    return camera_view_;
  }

 protected:
 private:
  bool Orbit(float _dx, float _dy);
  bool Zoom(float _d);
  bool Pan(float _dx, float _dy);

  float min_dist_ = 1.f;
  float max_dist_ = 100.f;
  float min_lat_ = -1.5f;
  float max_lat_ = 1.5f;
  float orbit_factor_ = .01f;
  float pan_factor_ = .002f;
  float zoom_factor_ = .5f;
  float distance_ = 20.f;
  float latitude_ = .2f;
  float longitude_ = 0.f;
  CameraView camera_view_ = {.fov = HMM_PI32 / 2.f};

  bool lshift_down_ = false;
};

}  // namespace flip