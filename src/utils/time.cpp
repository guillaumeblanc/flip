#include "flip/utils/time.h"

#include "imgui/imgui.h"

namespace flip {

Time TimeControl::Update(float _dt) {
  auto dt = 0.f;
  if (!freeze_) {
    if (fix_rate_) {
      dt = scale_ / fixed_rate_;
    } else {
      dt = _dt * scale_;
    }
  }
  elapsed_ += dt;

  return {elapsed_, dt, dt == 0.f ? 0.f : (1.f / dt)};
}

bool TimeControl::Gui() {
  ImGui::Checkbox("Freeze", &freeze_);
  ImGui::SameLine();
  ImGui::Checkbox("Fix update rate", &fix_rate_);
  if (ImGui::Button("Reset")) {
    fixed_rate_ = kFixedRateDefault;
    scale_ = 1.f;
  }
  ImGui::SameLine();
  if (fix_rate_) {
    ImGui::SliderFloat("Update rate", &fixed_rate_, 1.f, 200.f, "%.2f fps");
  } else {
    ImGui::SliderFloat("Time scale", &scale_, -10.f, 10.f, "%.2f",
                       ImGuiSliderFlags_Logarithmic);
  }
  return true;
}

}  // namespace flip