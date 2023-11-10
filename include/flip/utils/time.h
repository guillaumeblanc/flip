#pragma once

namespace flip {

struct Time {
  float elapsed;
  float dt;
  float inv_dt;
};

class TimeControl {
 public:
  Time Update(float _dt);

  bool Gui();

 private:
  // Current time, including scaling and freezes..
  float elapsed_ = 0.f;

  // Update time scale factor.
  float scale_ = 1;

  // Update time freeze state.
  bool freeze_ = false;

  // Fixed update rate, only applies to application update dt, not the real fps.
  const float kFixedRateDefault = 60.f;
  float fixed_rate_ = kFixedRateDefault;

  // Fixes update rat to a fixed value, instead of real_time.
  bool fix_rate_ = false;
};

}  // namespace flip