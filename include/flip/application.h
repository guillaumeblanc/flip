#pragma once

#include <memory>

// User is expected to implement this function to instantiate its
// flip::Application implementation.
namespace flip {
class Application;
}
extern std::unique_ptr<flip::Application> InstantiateApplication();

// Sokol forward declaration
struct sapp_event;

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
  Application(const Settings& _settings) : settings_{_settings} {}
  const auto& settings() const { return settings_; }

  // Enumeration of application update loop return modes.
  enum class LoopControl {
    kContinue,      // Continue with next loop.
    kBreak,         // Stop looping and return with EXIT_SUCCESS.
    kBreakFailure,  // Stop looping and return with EXIT_FAILURE.
  };

 private:
  virtual bool Initialize(bool _headless) { return true; }

  virtual LoopControl Update(float _time, float _dt, float _inv_dt) {
    return LoopControl::kContinue;
  }

  virtual bool Display(Renderer& _renderer, float _time, float _dt,
                       float _inv_dt) {
    return true;
  }
  virtual bool Menu() { return true; }
  virtual bool Gui() { return true; }

  // Return true if event was captured and shall not be forwarded further.
  virtual bool Event(const sapp_event& _event) { return false; }

  // Application global settings.
  const Settings settings_;

  // ApplicationCb is the only one allowed to call private interface
  // functions
  friend class ApplicationCb;
};
}  // namespace flip