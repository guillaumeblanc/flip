add_custom_command(
  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/../media/flip_2048.png"
  OUTPUT  "${CMAKE_CURRENT_BINARY_DIR}/media/texture.png"
  COMMAND ${CMAKE_COMMAND} -E make_directory media
  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/../media/flip_2048.png" "${CMAKE_CURRENT_BINARY_DIR}/media/texture.png"
  VERBATIM)

add_executable(texture main.cpp "${CMAKE_CURRENT_BINARY_DIR}/media/texture.png")
target_link_libraries(texture flip)
target_emscripten(texture)
add_test(NAME texture COMMAND texture "headless=true")