add_executable(custom main.cpp)
target_link_libraries(custom flip)
target_emscripten(custom)
add_test(NAME custom COMMAND custom "headless=true")
