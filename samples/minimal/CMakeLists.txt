add_executable(minimal main.cpp)
target_link_libraries(minimal flip)
target_emscripten(minimal)
add_test(NAME minimal COMMAND minimal "headless=true")
