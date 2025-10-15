#pragma once
#include "CompositeTransform.h"
#include <glm/gtc/matrix_transform.hpp>

class Rotation : public CompositeTransform {
private:
    float angle;      // Úhel rotace v radiánech
    glm::vec3 axis;   // Osa rotace (např. 0,0,1 pro Z osu)
public:
    Rotation(float angle, const glm::vec3& axis);
    // Implementace virtuální metody
    glm::mat4 getMatrix() const override;
};