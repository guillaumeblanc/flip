add_subdirectory(extern)

# Application
add_library(flip
  ${PROJECT_SOURCE_DIR}/include/flip/application.h
  ${PROJECT_SOURCE_DIR}/include/flip/camera.h
  ${PROJECT_SOURCE_DIR}/include/flip/math.h
  ${PROJECT_SOURCE_DIR}/include/flip/renderer.h
  ${PROJECT_SOURCE_DIR}/include/flip/imdraw.h
  ${PROJECT_SOURCE_DIR}/include/flip/utils/keyboard.h
  ${PROJECT_SOURCE_DIR}/include/flip/utils/loader.h
  ${PROJECT_SOURCE_DIR}/include/flip/utils/sokol_gfx.h
  application.cpp
  impl/imdrawer.h
  impl/imdrawer.cpp
  impl/imgui_font.h
  impl/imgui.h
  impl/imgui.cpp
  impl/factory.h
  impl/factory.cpp
  impl/orbit_camera.h
  impl/orbit_camera.cpp
  impl/renderer_impl.h
  impl/renderer_impl.cpp
  impl/shapes.h
  impl/shapes.cpp
  utils/keyboard.cpp
  utils/loader.cpp
  utils/sokol_gfx.cpp)
target_include_directories(flip PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(flip sokol hmm stb_image)