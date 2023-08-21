#pragma once

#include <memory>

// User is expected to implement this function to instantiate its
// flip::Application implementation.
namespace flip {
class Application;
}
extern std::unique_ptr<flip::Application> InstantiateApplication();

namespace flip {
class Renderer;

// Base application interface.
class Application {
 public:
  virtual ~Application() = default;

 protected:
  struct Settings {
    int width = 1280;
    int height = 720;
    int sample_count = 4;
    bool high_dpi = true;
    const char* title = "Flip application";
  };
  Application(const Settings& _settings) {}
  const auto& settings() const { return settings_; }

 private:
  // ApplicationCb is the only one allowed to call private interface
  // functions
  friend class ApplicationCb;

  virtual bool Initialize() { return true; }

  enum LoopControl {
    kContinue,      // Continue with next loop.
    kBreak,         // Stop looping and return with EXIT_SUCCESS.
    kBreakFailure,  // Stop looping and return with EXIT_FAILURE.
  };
  virtual LoopControl Update(float _time, float _dt, float _inv_dt) {
    return kContinue;
  }

  virtual bool Display(Renderer& _renderer) { return true; }
  virtual bool Menu() { return true; }

  const Settings settings_;
};
}  // namespace flip