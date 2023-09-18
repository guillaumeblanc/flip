add_custom_command(
  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/../media/flip_128.h"
  OUTPUT  "${CMAKE_CURRENT_SOURCE_DIR}/flip_128.h"
  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../media/flip_128.h" "${CMAKE_CURRENT_SOURCE_DIR}/flip_128.h"
  VERBATIM)

add_executable(shapes main.cpp flip_128.h)
target_link_libraries(shapes flip)
target_emscripten(shapes)
add_test(NAME shapes COMMAND shapes "headless=true")
