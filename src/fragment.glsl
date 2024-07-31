#version 330 core

uniform float height;
out vec4 FragColor;

void main() {
    // mix blue and red with height
    // full red at max height, full blue at min height, mix in between
    FragColor = vec4(height, 0.0f, 1.0f - height, 1.0f);
}