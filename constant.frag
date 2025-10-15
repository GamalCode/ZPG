#version 330

in vec3 worldPosition;
in vec3 worldNormal;

out vec4 fragColor;

void main() {
    fragColor = vec4(0.385, 0.647, 0.812, 1.0);
}