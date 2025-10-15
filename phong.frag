#version 330

in vec3 worldPosition;
in vec3 worldNormal;

uniform vec3 lightPosition;
uniform vec3 cameraPosition;

out vec4 fragColor;

void main() {
    // Object color
    vec4 objectColor = vec4(0.385, 0.647, 0.812, 1.0);
    
    // Ambient
    vec4 ambient = vec4(0.1, 0.1, 0.1, 1.0);
    
    // Diffuse (Lambert)
    vec3 norm = normalize(worldNormal);
    vec3 lightDir = normalize(lightPosition - worldPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec4 diffuse = diff * objectColor;
    
    // Specular (Phong)
    vec3 viewDir = normalize(cameraPosition - worldPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec4 specular = spec * vec4(1.0, 1.0, 1.0, 1.0);
    
    // I = Ia + Id + Is
    fragColor = ambient + diffuse + specular;
}