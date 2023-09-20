add_executable(split main.cpp)
target_link_libraries(split flip)
target_emscripten(split)
add_test(NAME split COMMAND split "headless=true")
