#include "flip/application.h"

// Implement the minimal flip::Application.
class Minimal : public flip::Application {
 public:
  Minimal() : flip::Application(Settings{.title = "Minimal"}) {}
};

// Application instantiation function
std::unique_ptr<flip::Application> InstantiateApplication() {
  return std::make_unique<Minimal>();
}
