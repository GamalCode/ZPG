#pragma once
#include "CompositeTransform.h"
#include <glm/gtc/matrix_transform.hpp>

class Translation : public CompositeTransform {
private:
    glm::vec3 translation;
public:
    Translation(const glm::vec3& t);
    Translation(float x, float y, float z);
    // Implementace virtuální metody
    glm::mat4 getMatrix() const override;
};