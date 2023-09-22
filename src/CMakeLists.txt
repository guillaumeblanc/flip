

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

# Stb image
add_library(stb_image extern/stb/stb_image.cpp ${PROJECT_SOURCE_DIR}/extern/stb/stb_image.h)
target_include_directories(stb_image PUBLIC ${PROJECT_SOURCE_DIR}/extern)
target_compile_definitions(stb_image PUBLIC STBI_NO_STDIO)

# Sokol
add_library(sokol
  $<IF:$<BOOL:${APPLE}>, extern/sokol/sokol.mm, extern/sokol/sokol.cpp>
  extern/sokol/sokol.h)
if(EMSCRIPTEN)
  target_compile_definitions(sokol PUBLIC SOKOL_GLES3)
  target_link_options(sokol PUBLIC -sUSE_WEBGL2=1)
  target_link_options(sokol PUBLIC -sMALLOC=emmalloc)
  target_link_options(sokol PUBLIC -sFILESYSTEM=0)
else()
  target_compile_definitions(sokol PUBLIC SOKOL_GLCORE33)
endif()
target_compile_definitions(sokol PUBLIC SOKOL_NO_ENTRY)
target_include_directories(sokol PUBLIC ${PROJECT_SOURCE_DIR}/extern)
target_include_directories(sokol PRIVATE ${PROJECT_SOURCE_DIR}/extern/sokol)

find_package(OpenGL REQUIRED)
target_link_libraries(sokol ${OPENGL_LIBRARIES} imgui)

if(APPLE)
  find_library(Cocoa Cocoa REQUIRED)
  find_library(QuartzCore QuartzCore REQUIRED)
  target_link_libraries(sokol ${QuartzCore} ${Cocoa})
endif()

if(UNIX AND NOT APPLE AND NOT EMSCRIPTEN)
  find_package(X11 REQUIRED)

  target_include_directories(sokol PRIVATE ${X11_INCLUDE_DIR})
  target_link_libraries(sokol ${X11_LIBRARIES} Xi Xcursor)
endif()

# Application
add_library(flip
  ${PROJECT_SOURCE_DIR}/include/flip/application.h
  ${PROJECT_SOURCE_DIR}/include/flip/camera.h
  ${PROJECT_SOURCE_DIR}/include/flip/math.h
  ${PROJECT_SOURCE_DIR}/include/flip/renderer.h
  ${PROJECT_SOURCE_DIR}/include/flip/imdraw.h
  ${PROJECT_SOURCE_DIR}/include/flip/utils/keyboard.h
  ${PROJECT_SOURCE_DIR}/include/flip/utils/loader.h
  ${PROJECT_SOURCE_DIR}/include/flip/utils/profile.h
  ${PROJECT_SOURCE_DIR}/include/flip/utils/sokol_gfx.h
  main.cpp
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
  utils/profile.cpp
  utils/sokol_gfx.cpp)
target_include_directories(flip PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(flip sokol hmm stb_image)