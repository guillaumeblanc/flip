
add_executable(minimal minimal.cpp)
target_link_libraries(minimal flip)
add_test(NAME minimal COMMAND minimal "headless=true")

add_executable(cubes cubes.cpp)
target_link_libraries(cubes flip)
add_test(NAME cubes COMMAND cubes "headless=true")