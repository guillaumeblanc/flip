#include "flip/application.h"
#include "flip/utils/keyboard.h"
#include "imgui/imgui.h"

// Implement the minimal flip::Application.
class Input : public flip::Application {
 public:
  Input() : flip::Application(Settings{.title = "Input"}) {}

 private:
  virtual LoopControl Update(const flip::Time& _time) override {
    if (keyboard_[SAPP_KEYCODE_Q].released()) {
      return LoopControl::kBreak;
    }
    return LoopControl::kContinue;
  }

  virtual bool Event(const sapp_event& _event) override {
    keyboard_.Event(_event);
    return false;
  }

  virtual bool Gui() override {
    if (ImGui::Begin("Keyboard state")) {
      if (ImGui::BeginTable("keyboard_state_table", 3,
                            ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders)) {
        ImGui::TableSetupColumn("Key");
        ImGui::TableSetupColumn("Down");
        ImGui::TableSetupColumn("Released");
        ImGui::TableHeadersRow();

        for (auto& it : keyboard_.keys()) {
          auto& key = it.second;
          ImGui::TableNextRow();

          ImGui::TableSetColumnIndex(0);
          ImGui::TextUnformatted(key.name);

          const ImVec4 kColors[] = {{1.f, 1.f, 1.f, 1.f}, {1.f, 0.f, 0.f, 1.f}};

          ImGui::TableSetColumnIndex(1);
          ImGui::TextColored(kColors[key.state.down],
                             key.state.down ? "True" : "False");

          ImGui::TableSetColumnIndex(2);
          ImGui::TextColored(kColors[key.state.released()],
                             key.state.released() ? "True" : "False");
        };

        ImGui::EndTable();
      }
    }
    ImGui::End();

    return true;
  }

  flip::Keyboard keyboard_;
};

// Application instantiation function
std::unique_ptr<flip::Application> InstantiateApplication() {
  return std::make_unique<Input>();
}
