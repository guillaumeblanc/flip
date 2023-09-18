function(target_emscripten _target)
  if(EMSCRIPTEN)
    set(FLIP_TARGET_NAME ${_target})
    configure_file("${PROJECT_SOURCE_DIR}/src/wasm.html" "${_target}.html")
    unset(FLIP_TARGET_NAME)

    set(FLIP_TARGET_SITE "${PROJECT_BINARY_DIR}/site/${_target}/")
    add_custom_command(TARGET ${_target} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E make_directory "media/" # in case it doesn't exist
      COMMAND ${CMAKE_COMMAND} -E make_directory "${FLIP_TARGET_SITE}"
      COMMAND ${CMAKE_COMMAND} -E copy_directory "media/" "${FLIP_TARGET_SITE}/media/"
      COMMAND ${CMAKE_COMMAND} -E copy "${_target}.html" "${FLIP_TARGET_SITE}/index.html"
      COMMAND ${CMAKE_COMMAND} -E copy "${_target}.wasm" "${FLIP_TARGET_SITE}"
      COMMAND ${CMAKE_COMMAND} -E copy "${_target}.js" "${FLIP_TARGET_SITE}"
      VERBATIM)
    unset(FLIP_SITE_TARGET)
  endif()
endfunction()

add_executable(minimal minimal.cpp)
target_link_libraries(minimal flip)
target_emscripten(minimal)
add_test(NAME minimal COMMAND minimal "headless=true")

add_executable(cubes cubes.cpp media/flip_128.h)
target_link_libraries(cubes flip)
target_emscripten(cubes)
add_test(NAME cubes COMMAND cubes "headless=true")

add_executable(input input.cpp)
target_link_libraries(input flip)
target_emscripten(input)
add_test(NAME input COMMAND input "headless=true")

add_executable(imdrawing imdrawing.cpp)
target_link_libraries(imdrawing flip)
target_emscripten(imdrawing)
add_test(NAME imdrawing COMMAND imdrawing "headless=true")

add_executable(custom custom.cpp)
target_link_libraries(custom flip)
target_emscripten(custom)
add_test(NAME custom COMMAND custom "headless=true")

add_custom_command(
  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/media/flip_512.png"
  OUTPUT  "${CMAKE_CURRENT_BINARY_DIR}/media/texture.png"
  COMMAND ${CMAKE_COMMAND} -E make_directory media
  COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/media/flip_512.png" "${CMAKE_CURRENT_BINARY_DIR}/media/texture.png"
  VERBATIM)

add_executable(texture texture.cpp "${CMAKE_CURRENT_BINARY_DIR}/media/texture.png")
target_link_libraries(texture flip)
target_emscripten(texture)
add_test(NAME texture COMMAND texture "headless=true")