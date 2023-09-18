add_executable(input main.cpp)
target_link_libraries(input flip)
target_emscripten(input)
add_test(NAME input COMMAND input "headless=true")
