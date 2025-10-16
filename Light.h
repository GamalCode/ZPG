#pragma once
#include <glm/glm.hpp>

class Light {
private:
    glm::vec3 position;
    glm::vec3 color;

public:
    Light(const glm::vec3& pos = glm::vec3(0.0f),
        const glm::vec3& col = glm::vec3(1.0f));

    glm::vec3 getPosition() const { return position; }
    glm::vec3 getColor() const { return color; }

    void setPosition(const glm::vec3& pos) { position = pos; }
    void setColor(const glm::vec3& col) { color = col; }
};