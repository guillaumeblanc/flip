#include <cstdlib>
#include <memory>

#include "flip/application.h"
#include "flip/camera.h"
#include "flip/imgui.h"
#include "flip/renderer.h"
#include "impl/factory.h"

// Sokol library
#include "sokol/sokol_app.h"
#include "sokol/sokol_args.h"
#include "sokol/sokol_log.h"
#include "sokol/sokol_time.h"

namespace flip {

class ApplicationCb {
 public:
  ApplicationCb(bool _headless)
      : renderer_(_headless ? nullptr : Factory().InstantiateRenderer()),
        camera_(_headless ? nullptr : Factory().InstantiateCamera()),
        application_(InstantiateApplication()) {}

  bool headless() const { return renderer_ == nullptr; }

  int Run() {
    if (!headless()) {
      const auto& settings = application_->settings();

      // Setup and run sapp
      sapp_run(sapp_desc{
          .user_data = this,
          .init_userdata_cb =
              [](void* _ud) { static_cast<flip::ApplicationCb*>(_ud)->Init(); },
          .frame_userdata_cb =
              [](void* _ud) {
                static_cast<flip::ApplicationCb*>(_ud)->Frame();
              },
          .cleanup_userdata_cb =
              [](void* _ud) {
                static_cast<flip::ApplicationCb*>(_ud)->Cleanup();
              },
          .event_userdata_cb =
              [](const sapp_event* _e, void* _ud) {
                static_cast<flip::ApplicationCb*>(_ud)->Event(*_e);
              },
          .width = settings.width,
          .height = settings.height,
          .sample_count = settings.sample_count,
          .high_dpi = settings.high_dpi,
          .window_title = settings.title,
          .logger = {.func = slog_func}});

      // sapp_run never returns on some platforms. Cleanup must be done in the
      // cb.
    } else {
      // Implement a basic headless loop
      Init();
      for (size_t i = 0; i < 10 && !exit_; ++i) {
        Frame();
      }
      Cleanup();
    }

    return exit_code_;
  }

  void Init() {
    bool success = true;

    // Time management
    stm_setup();

    if (!headless()) {
      success &= renderer_->Initialize();
      success &= camera_->Initialize();
    }

    success &= application_->Initialize(headless());

    if (!success) {
      RequestExit(success);
    }
  }

  void Cleanup() {
    // Release resources (symmetrical to constructor & initialize)
    application_ = nullptr;
    camera_ = nullptr;
    renderer_ = nullptr;

    // Shuting down sargs here is not symmetrical with initialization, but the
    // only way to make sure it's done on all platforms.
    sargs_shutdown();
  }

  void Event(const sapp_event& _event) {
    if (!headless()) {
      bool captured = false;  // Only fw event if not captured
      captured = !captured && renderer_->Event(_event);
      captured = !captured && camera_->Event(_event);
      captured = !captured && application_->Event(_event);
    }
  }

  void Frame() {
    bool success = true;

    // Updates time.
    const auto dt = static_cast<float>(stm_sec(stm_laptime(&last_time_)));
    const auto inv_dt = dt == 0.f ? 0.f : (1.f / dt);
    const auto time = static_cast<float>(stm_sec(last_time_));

    // Updates application
    const auto control = application_->Update(time, dt, inv_dt);
    success &= control != Application::LoopControl::kBreakFailure;

    // Renders application
    if (renderer_) {
      success &= Display(time, dt, inv_dt);
    }

    // Manages exit
    if (!success || control != Application::LoopControl::kContinue) {
      RequestExit(success);
    }
  }

  bool Display(float _time, float _dt, float _inv_dt) {
    bool success = true;
    success &= camera_->Update(_time, _dt, _inv_dt);

    {  // Default Rendering pass raii
      auto pass = Renderer::DefaultPass(*renderer_, camera_->GetCameraView());

      success &= renderer_->DrawAxes(make_span(HMM_M4D(1)));

      // Overloaded application display
      success &= application_->Display(*renderer_);

      {  // Imgui frame raii
        auto imgui_frame = Imgui::Frame(renderer_->imgui());

        // Gui
        success &= application_->Gui();

        {  // Imgui menu raii
          auto menu = Imgui::MainMenu(imgui_frame);

          success &= application_->Menu();
          success &= camera_->Menu();
          success &= renderer_->Menu();
        }
      }
    }
    return success;
  }

  void RequestExit(bool _success) {
    exit_ = true;
    exit_code_ = _success ? EXIT_SUCCESS : EXIT_FAILURE;

    sapp_quit();
  }

 private:
  // Resources, order is important for destruction order
  std::unique_ptr<Renderer> renderer_;
  std::unique_ptr<Camera> camera_;
  std::unique_ptr<Application> application_;

  // Time management
  uint64_t last_time_ = 0;

  // Exit management.
  bool exit_ = false;
  int exit_code_ = EXIT_SUCCESS;
};
}  // namespace flip

int main(int _argc, char* _argv[]) {
  // Setup sargs (cli arguments)
  sargs_setup(sargs_desc{.argc = _argc, .argv = _argv});

  bool headless = false;
  if (sargs_exists("headless")) {
    headless = sargs_boolean("headless");
  }

  // Run application loop (init / loop / cleanup)
  return flip::ApplicationCb(headless).Run();
}
