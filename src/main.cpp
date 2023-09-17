#include <cassert>
#include <cstdlib>
#include <filesystem>
#include <memory>

#include "flip/application.h"
#include "flip/camera.h"
#include "flip/renderer.h"
#include "impl/factory.h"

// Sokol library
#include "sokol/sokol_app.h"
#include "sokol/sokol_args.h"
#include "sokol/sokol_fetch.h"
#include "sokol/sokol_log.h"
#include "sokol/sokol_time.h"

#ifdef __APPLE__
#include <unistd.h>
#endif  // __APPLE__

namespace flip {

class ApplicationCb {
 public:
  ApplicationCb() : application_(InstantiateApplication()) {
    if (sargs_exists("headless")) {
      headless_ = sargs_boolean("headless");
    }
  }
  ~ApplicationCb() = default;

  static int Run(int _argc, char* _argv[]) {
    // Setup sargs (cli arguments) so application can use it
    sargs_setup(sargs_desc{.argc = _argc, .argv = _argv});

    auto app_cb = std::make_unique<ApplicationCb>();

    if (app_cb->headless_) {
      // Implement a basic headless loop
      app_cb->Init();
      for (size_t i = 0; i < 10 && !app_cb->exit_; ++i) {
        app_cb->Frame();
      }
      app_cb.release()->Cleanup();
    } else {
      const auto& settings = app_cb->application_->settings();

      // Setup and run sapp
      sapp_run(sapp_desc{
          .user_data = app_cb.release(),
          .init_userdata_cb =
              [](void* _ud) { static_cast<ApplicationCb*>(_ud)->Init(); },
          .frame_userdata_cb =
              [](void* _ud) { static_cast<ApplicationCb*>(_ud)->Frame(); },
          .cleanup_userdata_cb =
              [](void* _ud) { static_cast<ApplicationCb*>(_ud)->Cleanup(); },
          .event_userdata_cb =
              [](const sapp_event* _e, void* _ud) {
                static_cast<ApplicationCb*>(_ud)->Event(*_e);
              },
          .width = settings.width,
          .height = settings.height,
          .sample_count = settings.sample_count,
          .high_dpi = settings.high_dpi,
          .window_title = settings.title,
          .logger = {.func = slog_func},
      });

      // sapp_run never behaves differently depending on the platform:
      // - returns immediately for emscripten.
      // - never returns on macos.
      // - on exit on other platforms.
      //  Hence all destruction (inc ApplicationCb) must be done in the cleanup
      //  cb.
    }

    return ApplicationCb::exit_code_;
  }

 protected:
  void Init() {
    bool success = true;

    // Time management
    stm_setup();

    // Fetching
    const auto& app_desc = sapp_query_desc();
    sfetch_setup(
        sfetch_desc_t{.logger = {.func = app_desc.logger.func,
                                 .user_data = app_desc.logger.user_data}});

    if (!headless_) {
      renderer_ = Factory().InstantiateRenderer();
      camera_ = Factory().InstantiateCamera();
    }

    success &= application_->Initialize(headless_);

    if (!success) {
      RequestExit(success);
    }
  }

  void Cleanup() {
    // Release resources (symmetrical to constructor & initialize)
    application_ = nullptr;
    camera_ = nullptr;
    renderer_ = nullptr;

    // Stops fetching
    sfetch_shutdown();

    // Shuting down sargs here is not symmetrical with initialization, but the
    // only way to make sure it's done on all platforms.
    sargs_shutdown();

    // Finally delete this before exit.
    delete this;
  }

  void Event(const sapp_event& _event) {
    bool captured = false;  // Only fw event if not captured
    captured = !captured && renderer_->Event(_event);
    captured = !captured && camera_->Event(_event);
    captured = !captured && application_->Event(_event);
  }

  void Frame() {
    bool success = true;

    // Ticks fetching
    sfetch_dowork();

    // Updates time.
    const auto dt = static_cast<float>(stm_sec(stm_laptime(&last_time_)));
    const auto inv_dt = dt == 0.f ? 0.f : (1.f / dt);
    const auto time = static_cast<float>(stm_sec(last_time_));

    // Updates application
    const auto control = application_->Update(time, dt, inv_dt);
    success &= control != Application::LoopControl::kBreakFailure;

    // Renders application
    if (!headless_) {
      success &= Display(time, dt, inv_dt);
    }

    // Manages exit
    if (!success || control != Application::LoopControl::kContinue) {
      RequestExit(success);
    }
  }

  bool Display(float _time, float _dt, float _inv_dt) {
    assert(!headless_);

    bool success = true;
    success &= camera_->Update(_time, _dt, _inv_dt);

    {  // Default Rendering pass raii
      auto pass = Renderer::DefaultPass(*renderer_, camera_->GetCameraView());

      // Overloaded application display
      success &= application_->Display(*renderer_);

      success &= renderer_->DrawGrids({&kIdentity4, 1}, 20);
      success &= renderer_->DrawAxes({&kIdentity4, 1});

      // Gui
      success &= application_->Gui();

      {  // Imgui menu
        success &= application_->Menu();
        success &= camera_->Menu();
        success &= renderer_->Menu();
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

  // Does application has a windows (head)
  bool headless_ = false;

  // Exit management.
  bool exit_ = false;
  static int exit_code_;
};

int ApplicationCb::exit_code_ = EXIT_SUCCESS;
}  // namespace flip

int main(int _argc, char* _argv[]) {
#ifdef __APPLE__
  // On OSX, when run from Finder, working path is the root path. This does
  // not allow to load resources from relative path. The workaround is to
  // change the working directory to application directory. The proper
  // solution would probably be to use bundles and load data from resource
  // folder.
  auto path = std::filesystem::path(_argv[0]);
  chdir(path.remove_filename().c_str());
#endif  // __APPLE__

  // Run application loop (init / loop / cleanup)
  return flip::ApplicationCb::Run(_argc, _argv);
}
