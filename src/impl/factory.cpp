#include "factory.h"

#include "imgui.h"
#include "orbit_camera.h"
#include "renderer_impl.h"

namespace flip {

std::unique_ptr<Camera> Factory::InstantiateCamera() {
  return std::make_unique<OrbitCamera>();
}
std::unique_ptr<Renderer> Factory::InstantiateRenderer() {
  return std::make_unique<RendererImpl>();
}
std::unique_ptr<Imgui> Factory::InstantiateImgui() {
  return std::make_unique<Imgui>();
}

}  // namespace flip