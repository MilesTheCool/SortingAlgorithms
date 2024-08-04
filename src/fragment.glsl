#version 330 core

uniform vec3 color;
out vec4 FragColor;

void main() {
    // mix blue and red with height
    // full red at max height, full blue at min height, mix in between
    FragColor = vec4(color, 1.0f);
}