#include "factory.h"

#include "orbit_camera.h"
#include "renderer_impl.h"

namespace flip {

std::unique_ptr<Camera> Factory::InstantiateCamera() {
  return std::make_unique<OrbitCamera>();
}
std::unique_ptr<Renderer> Factory::InstantiateRenderer() {
  return std::make_unique<RendererImpl>();
}

}  // namespace flip