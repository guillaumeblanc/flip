add_executable(imdraw main.cpp)
target_link_libraries(imdraw flip)
target_emscripten(imdraw)
add_test(NAME imdraw COMMAND imdraw "headless=true")
