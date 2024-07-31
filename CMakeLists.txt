cmake_minimum_required(VERSION 3.5.0)
project(sorting_algorithm_displayer VERSION 0.1.0)
cmake_policy(SET CMP0072 NEW)

add_executable(sorting_algorithm_displayer
    src/main.cpp 
    src/glad.c
)

target_link_libraries(sorting_algorithm_displayer
    glfw
)