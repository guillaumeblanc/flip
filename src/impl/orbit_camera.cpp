#include "orbit_camera.h"

// math
#include <algorithm>
#include <cmath>

#include "hmm/HandmadeMath.h"
#include "imgui/imgui.h"
#include "sokol/sokol_app.h"

namespace flip {

bool OrbitCamera::Update(float _time, float _dt, float _inv_dt) {
  bool success = true;

  // Camera to eye vector
  const auto cln = cosf(longitude_);
  const auto sln = sinf(longitude_);
  const auto cla = cosf(latitude_);
  const auto sla = sinf(latitude_);
  const auto c2e = HMM_Vec3{cla * sln, sla, cla * cln};

  // Eye position
  camera_view_.eye = camera_view_.center + c2e * distance_;

  return success;
}

bool OrbitCamera::Menu() {
  if (ImGui::BeginMenu("Camera")) {
    ImGui::SliderAngle("Field of view", &camera_view_.fov, 60, 120);
    ImGui::EndMenu();
  }
  return true;
}

bool OrbitCamera::Orbit(float _dx, float _dy) {
  longitude_ -= _dx * orbit_factor_;

  while (longitude_ < 0) {
    longitude_ += HMM_PI32 * 2;
  }
  while (longitude_ > HMM_PI32 * 2) {
    longitude_ -= HMM_PI32 * 2;
  }

  latitude_ = std::clamp(latitude_ + _dy * orbit_factor_, min_lat_, max_lat_);
  return true;
}

bool OrbitCamera::Pan(float _dx, float _dy) {
  const auto cln = cosf(longitude_);
  const auto sln = sinf(longitude_);
  const auto cla = cosf(latitude_);
  const auto sla = sinf(latitude_);

  const auto left = HMM_Vec3{-cln, 0, sln};
  const auto up = HMM_Vec3{-sla * sln, cla, -sla * cln};

  camera_view_.center += (left * _dx + up * _dy) * pan_factor_ * distance_;
  return true;
}

bool OrbitCamera::Zoom(float _d) {
  distance_ = std::clamp(distance_ + _d * zoom_factor_, min_dist_, max_dist_);
  return true;
}

bool OrbitCamera::Event(const sapp_event& _event) {
  switch (_event.type) {
    case SAPP_EVENTTYPE_MOUSE_DOWN:
      if (_event.mouse_button == SAPP_MOUSEBUTTON_LEFT) {
        sapp_lock_mouse(true);
      }
      break;
    case SAPP_EVENTTYPE_MOUSE_UP:
      if (_event.mouse_button == SAPP_MOUSEBUTTON_LEFT) {
        sapp_lock_mouse(false);
      }
      break;
    case SAPP_EVENTTYPE_MOUSE_SCROLL: {
      Zoom(_event.scroll_y);
      return true;
    }
    case SAPP_EVENTTYPE_MOUSE_MOVE:
      if (sapp_mouse_locked()) {
        if (lshift_down_) {
          Pan(_event.mouse_dx, _event.mouse_dy);
        } else {
          Orbit(_event.mouse_dx, _event.mouse_dy);
        }
        return true;
      }
      break;
    case SAPP_EVENTTYPE_KEY_DOWN: {
      lshift_down_ |= _event.key_code == SAPP_KEYCODE_LEFT_SHIFT;
    } break;
    case SAPP_EVENTTYPE_KEY_UP: {
      lshift_down_ &= _event.key_code != SAPP_KEYCODE_LEFT_SHIFT;
    } break;
    default:
      break;
  }
  return false;
}

}  // namespace flip