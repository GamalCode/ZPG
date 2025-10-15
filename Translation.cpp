#include "Translation.h"

Translation::Translation(const glm::vec3& t) : translation(t) {}

Translation::Translation(float x, float y, float z) : translation(x, y, z) {}

glm::mat4 Translation::getMatrix() const {
    return glm::translate(glm::mat4(1.0f), translation);
}