cmake_minimum_required(VERSION 3.0.0)
project(opengl_pong_game VERSION 0.1.0)
cmake_policy(SET CMP0072 NEW)

add_executable(opengl_pong_game
    src/main.cpp 
    src/glad.c
)

target_link_libraries(opengl_pong_game
    glfw
)