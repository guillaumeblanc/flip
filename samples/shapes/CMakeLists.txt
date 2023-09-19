add_custom_command(
  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/../media/flip_128.h"
  OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/logo.h"
  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../media/flip_128.h" "${CMAKE_CURRENT_SOURCE_DIR}/logo.h"
  VERBATIM)

add_executable(shapes main.cpp logo.h)
target_link_libraries(shapes flip)
target_emscripten(shapes)
add_test(NAME shapes COMMAND shapes "headless=true")
