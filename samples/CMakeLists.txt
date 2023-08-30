
add_executable(minimal minimal.cpp)
target_link_libraries(minimal flip)
add_test(NAME minimal COMMAND minimal "headless=true")

add_executable(cubes cubes.cpp)
target_link_libraries(cubes flip)
add_test(NAME cubes COMMAND cubes "headless=true")

add_executable(input input.cpp)
target_link_libraries(input flip)
add_test(NAME input COMMAND input "headless=true")

add_executable(im_draw im_draw.cpp)
target_link_libraries(im_draw flip)
add_test(NAME im_draw COMMAND im_draw "headless=true")

add_executable(custom custom.cpp)
target_link_libraries(custom flip)
add_test(NAME custom COMMAND custom "headless=true")