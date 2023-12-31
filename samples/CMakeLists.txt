# Changes runtime output dir so resources can be accessed with a cross platform relative path
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ".")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ".")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ".")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ".")

set(FLIP_SITE "${PROJECT_BINARY_DIR}/site/")
file(MAKE_DIRECTORY ${FLIP_SITE})

function(target_emscripten _target)
  if(EMSCRIPTEN)
    set(FLIP_TARGET_NAME ${_target})
    configure_file("${PROJECT_SOURCE_DIR}/src/wasm.html" "${_target}.html")
    unset(FLIP_TARGET_NAME)

    set(FLIP_TARGET_SITE "${FLIP_SITE}${_target}/")
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

add_subdirectory(custom)
add_subdirectory(imdraw)
add_subdirectory(input)
add_subdirectory(minimal)
add_subdirectory(shapes)
add_subdirectory(split)
add_subdirectory(texture)