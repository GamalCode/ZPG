#include "Scale.h"

Scale::Scale(const glm::vec3& s) : scale(s) {}

Scale::Scale(float x, float y, float z) : scale(x, y, z) {}

Scale::Scale(float uniform) : scale(uniform, uniform, uniform) {}

glm::mat4 Scale::getMatrix() const {
    return glm::scale(glm::mat4(1.0f), scale);
}