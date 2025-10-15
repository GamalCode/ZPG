#version 330

layout(location=0) in vec3 vp;
layout(location=1) in vec3 vn;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 worldPosition;
out vec3 worldNormal;

void main() {
    vec4 wp = modelMatrix * vec4(vp, 1.0);
    worldPosition = wp.xyz / wp.w;
    worldNormal = mat3(transpose(inverse(modelMatrix))) * vn;
    
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vp, 1.0);
}