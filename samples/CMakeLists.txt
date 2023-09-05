
add_executable(minimal minimal.cpp)
target_link_libraries(minimal flip)
add_test(NAME minimal COMMAND minimal "headless=true")

add_executable(cubes cubes.cpp)
target_link_libraries(cubes flip)
add_test(NAME cubes COMMAND cubes "headless=true")

add_executable(input input.cpp)
target_link_libraries(input flip)
add_test(NAME input COMMAND input "headless=true")

add_executable(imdrawing imdrawing.cpp)
target_link_libraries(imdrawing flip)
add_test(NAME imdrawing COMMAND imdrawing "headless=true")

add_executable(custom custom.cpp)
target_link_libraries(custom flip)
add_test(NAME custom COMMAND custom "headless=true")

add_custom_command(
  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/media/flip_512.png"
  OUTPUT  "${CMAKE_CURRENT_BINARY_DIR}/media/texture.png"
  COMMAND ${CMAKE_COMMAND} -E make_directory media
  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/media/flip_512.png" "${CMAKE_CURRENT_BINARY_DIR}/media/texture.png"
  VERBATIM)

add_executable(texture texture.cpp "${CMAKE_CURRENT_BINARY_DIR}/media/texture.png")
target_link_libraries(texture flip)
add_test(NAME texture COMMAND texture "headless=true")