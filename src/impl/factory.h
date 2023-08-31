#pragma once

#include <memory>

namespace flip {
class Camera;
class Renderer;

struct Factory {
  std::unique_ptr<Camera> InstantiateCamera();
  std::unique_ptr<Renderer> InstantiateRenderer();
};
}  // namespace flip