#include "Rotation.h"

Rotation::Rotation(float angle, const glm::vec3& axis)
    : angle(angle), axis(axis) {
}

glm::mat4 Rotation::getMatrix() const {
    return glm::rotate(glm::mat4(1.0f), angle, axis);
}