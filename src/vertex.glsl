#version 330 core
layout (location = 0) in vec3 pos;

uniform mat4 trans;
uniform mat4 perspective;

void main() {
    gl_Position = perspective * trans * vec4(pos, 1.0);
}
