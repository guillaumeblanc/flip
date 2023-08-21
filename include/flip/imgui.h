#pragma once

struct sapp_event;

namespace flip {

// Base Renderer interface
class Imgui {
 public:
  virtual ~Imgui() = default;

  virtual bool Initialize() { return true; }
  virtual bool Event(const sapp_event& _event) { return false; }
  virtual bool Menu() { return true; }

  // RAII to begin/end the frame
  class Frame {
   public:
    Frame(Imgui& _imgui) : imgui_(_imgui) { _imgui.BeginFrame(); }
    ~Frame() { imgui_.EndFrame(); }

    Imgui& imgui_;
  };

  // RAII to begin/end the main menu
  class MainMenu {
   public:
    MainMenu(Frame& _frame) : imgui_(_frame.imgui_) { imgui_.BeginMainMenu(); }
    ~MainMenu() { imgui_.EndMainMenu(); }

   private:
    Imgui& imgui_;
  };

 protected:
 private:
  virtual void BeginFrame() = 0;
  virtual void EndFrame() = 0;

  virtual void BeginMainMenu() = 0;
  virtual void EndMainMenu() = 0;
};

}  // namespace flip