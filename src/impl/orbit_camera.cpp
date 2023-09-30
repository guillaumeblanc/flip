#include "orbit_camera.h"

// math
#include <algorithm>
#include <cmath>

#include "flip/math.h"
#include "imgui/imgui.h"

namespace flip {

bool OrbitCamera::Update(const Time& _time) {
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
    ImGui::SliderAngle("Field of view", &camera_view_.fov, 40, 140);
    ImGui::LabelText("Controls",
                     "- LMB: Orbit\n- LMB + Shift: Pan\n- LMB + CTRL: Move "
                     "horizontally\n- Scroll: Zoom");
    ImGui::EndMenu();
  }
  return true;
}

bool OrbitCamera::Orbit(const HMM_Vec2& _delta) {
  longitude_ -= _delta.X;

  while (longitude_ < 0) {
    longitude_ += flip::k2Pi;
  }
  while (longitude_ > flip::k2Pi) {
    longitude_ -= flip::k2Pi;
  }

  latitude_ = std::clamp(latitude_ + _delta.Y, kMinLat, kMaxLat);
  return true;
}

bool OrbitCamera::Pan(const HMM_Vec2& _delta) {
  const auto cln = cosf(longitude_);
  const auto sln = sinf(longitude_);
  const auto cla = cosf(latitude_);
  const auto sla = sinf(latitude_);

  const auto left = HMM_NormV3(HMM_Vec3{-cln, 0, sln});
  const auto up = HMM_Vec3{-sla * sln, cla, -sla * cln};

  camera_view_.center += (left * _delta.X + up * _delta.Y) * distance_;
  return true;
}

bool OrbitCamera::Move(const HMM_Vec2& _delta) {
  const auto cln = cosf(longitude_);
  const auto sln = sinf(longitude_);

  const auto left_h = HMM_NormV3(HMM_Vec3{-cln, 0, sln});
  const auto fw_h = HMM_NormV3(HMM_Vec3{-sln, 0, -cln});

  camera_view_.center += (left_h * _delta.X + fw_h * _delta.Y) * distance_;
  return true;
}

bool OrbitCamera::Zoom(float _d) {
  distance_ = std::clamp(distance_ + _d, kMinDist, kMaxDist);
  return true;
}

bool OrbitCamera::Event(const sapp_event& _event) {
  switch (_event.type) {
    case SAPP_EVENTTYPE_MOUSE_DOWN:
      if (_event.mouse_button == SAPP_MOUSEBUTTON_LEFT) {
        sapp_lock_mouse(true);
        return true;
      }
      break;
    case SAPP_EVENTTYPE_MOUSE_UP:
      if (_event.mouse_button == SAPP_MOUSEBUTTON_LEFT) {
        sapp_lock_mouse(false);
        return true;
      }
    case SAPP_EVENTTYPE_MOUSE_SCROLL:
      Zoom(_event.scroll_y * kZoomFactor);
      return true;
    case SAPP_EVENTTYPE_MOUSE_MOVE:
      if (sapp_mouse_locked()) {
        const auto delta = HMM_Vec2{_event.mouse_dx, _event.mouse_dy};
        if (_event.modifiers & SAPP_MODIFIER_CTRL) {
          return Move(delta * kMoveFactor);
        } else if (_event.modifiers & SAPP_MODIFIER_SHIFT) {
          return Pan(delta * kPanFactor);
        } else {
          return Orbit(delta * kOrbitFactor);
        }
      }
      break;
    case SAPP_EVENTTYPE_TOUCHES_BEGAN:
      for (int i = 0; i < _event.num_touches; ++i) {
        const auto& touch = _event.touches[i];
        last_touches_[touch.identifier] = HMM_Vec2{touch.pos_x, touch.pos_y};
      }
      break;
    case SAPP_EVENTTYPE_TOUCHES_MOVED:
      if (_event.num_touches == 1) {
        const auto& touch = _event.touches[0];
        const auto& last_touch = last_touches_[touch.identifier];
        const auto mouse_offset =
            HMM_Vec2{touch.pos_x - last_touch.X, touch.pos_y - last_touch.Y};
        Orbit(mouse_offset * kOrbitTouchFactor);
      } else if (_event.num_touches == 2) {
        const auto v0 =
            HMM_Vec2{_event.touches[0].pos_x, _event.touches[0].pos_y};
        const auto v1 =
            HMM_Vec2{_event.touches[1].pos_x, _event.touches[1].pos_y};
        const auto& pv0 = last_touches_[_event.touches[0].identifier];
        const auto& pv1 = last_touches_[_event.touches[1].identifier];

        // Pinch (distance change between two touch points)
        Zoom((HMM_LenV2(pv1 - pv0) - HMM_LenV2(v1 - v0)) * kZoomTouchFactor);

        // Drag (movement from the middle of two touch points)
        const auto dm = (v1 + v0 - pv1 - pv0) / 2;
        Pan(dm * kPanTouchFactor);
      }

      // Updates all touch coords
      for (int i = 0; i < _event.num_touches; ++i) {
        const auto& touch = _event.touches[i];
        last_touches_[touch.identifier] = HMM_Vec2{touch.pos_x, touch.pos_y};
      }
      break;
    default:
      break;
  }
  return false;
}

}  // namespace flip