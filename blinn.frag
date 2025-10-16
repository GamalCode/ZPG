#version 330

in vec3 worldPosition;
in vec3 worldNormal;

uniform vec3 lightPosition;
uniform vec3 cameraPosition;

out vec4 fragColor;

void main() {
    vec4 objectColor = vec4(0.385, 0.647, 0.812, 1.0);
    vec4 ambient = vec4(0.1, 0.1, 0.1, 1.0);
    
    // Diffuse
    vec3 norm = normalize(worldNormal);
    vec3 lightDir = normalize(lightPosition - worldPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = diff * objectColor;
    
    // Blinn-Phong specular
    vec3 viewDir = normalize(cameraPosition - worldPosition);
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfDir), 0.0), 32.0);
    vec4 specular = spec * vec4(1.0, 1.0, 1.0, 1.0);
    
    fragColor = ambient + diffuse + specular;
}