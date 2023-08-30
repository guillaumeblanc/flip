#include "flip/application.h"
#include "flip/renderer.h"
// Implement the minimal flip::Application.
class ImDraw : public flip::Application {
 public:
  ImDraw() : flip::Application(Settings{.title = "ImDraw"}) {}

  virtual bool Display(flip::Renderer& _renderer) override {
    // auto drawer = flip::Renderer::ImDrawer{_renderer};
    return true;
  }
};

// Application instantiation function
std::unique_ptr<flip::Application> InstantiateApplication() {
  return std::make_unique<ImDraw>();
}
