# Dear imgui
add_library(imgui
  ${PROJECT_SOURCE_DIR}/extern/imgui/imconfig.h
  ${PROJECT_SOURCE_DIR}/extern/imgui/imgui.cpp
  ${PROJECT_SOURCE_DIR}/extern/imgui/imgui.h
  ${PROJECT_SOURCE_DIR}/extern/imgui/imgui_draw.cpp
  ${PROJECT_SOURCE_DIR}/extern/imgui/imgui_internal.h
  ${PROJECT_SOURCE_DIR}/extern/imgui/imgui_tables.cpp
  ${PROJECT_SOURCE_DIR}/extern/imgui/imgui_widgets.cpp
  ${PROJECT_SOURCE_DIR}/extern/imgui/imstb_rectpack.h
  ${PROJECT_SOURCE_DIR}/extern/imgui/imstb_textedit.h
  ${PROJECT_SOURCE_DIR}/extern/imgui/imstb_truetype.h)
target_include_directories(imgui PUBLIC ${PROJECT_SOURCE_DIR}/extern)
target_include_directories(imgui PRIVATE ${PROJECT_SOURCE_DIR}/extern/imgui)

# Handmade math
add_library(hmm INTERFACE ${PROJECT_SOURCE_DIR}/extern/hmm/HandmadeMath.h)
target_include_directories(hmm INTERFACE ${PROJECT_SOURCE_DIR}/extern)

# Sokol
add_library(sokol
  $<IF:$<BOOL:${APPLE}>, impl/sokol_impl.mm, impl/sokol_impl.cpp>
  impl/sokol_impl.h)
target_compile_definitions(sokol PUBLIC SOKOL_GLCORE33 SOKOL_NO_ENTRY)
target_include_directories(sokol PUBLIC ${PROJECT_SOURCE_DIR}/extern)
target_include_directories(sokol PRIVATE ${PROJECT_SOURCE_DIR}/extern/sokol)

find_package(OpenGL REQUIRED)
target_link_libraries(sokol ${OPENGL_LIBRARIES} imgui)

if(APPLE)
  find_library(Cocoa Cocoa REQUIRED)
  find_library(QuartzCore QuartzCore REQUIRED)
  target_link_libraries(sokol ${QuartzCore} ${Cocoa})
endif()

if(UNIX AND NOT APPLE)
  find_package(X11 REQUIRED)

  target_include_directories(sokol PRIVATE ${X11_INCLUDE_DIR})
  target_link_libraries(sokol ${X11_LIBRARIES} Xi Xcursor)
endif()

# Application
add_library(flip
  ${PROJECT_SOURCE_DIR}/include/flip/application.h
  ${PROJECT_SOURCE_DIR}/include/flip/camera.h
  ${PROJECT_SOURCE_DIR}/include/flip/imgui.h
  ${PROJECT_SOURCE_DIR}/include/flip/renderer.h
  ${PROJECT_SOURCE_DIR}/include/flip/utils/keyboard.h
  ${PROJECT_SOURCE_DIR}/include/flip/utils/sokol_gfx.h
  main.cpp
  impl/imgui_font.h
  impl/imgui_impl.h
  impl/imgui_impl.cpp
  impl/factory.h
  impl/factory.cpp
  impl/orbit_camera.h
  impl/orbit_camera.cpp
  impl/renderer_impl.h
  impl/renderer_impl.cpp
  impl/shapes.h
  impl/shapes.cpp
  utils/keyboard.cpp
  utils/sokol_gfx.cpp)
target_include_directories(flip PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(flip sokol hmm)