#pragma once

#include <memory>

namespace flip {
class Camera;
class Renderer;
class Imgui;

struct Factory {
  std::unique_ptr<Camera> InstantiateCamera();
  std::unique_ptr<Renderer> InstantiateRenderer();
  std::unique_ptr<Imgui> InstantiateImgui();
};
}  // namespace flip