#pragma once
#include "CompositeTransform.h"
#include <glm/gtc/matrix_transform.hpp>

class Scale : public CompositeTransform {
private:
    glm::vec3 scale;
public:
    Scale(const glm::vec3& s);
    Scale(float x, float y, float z);
    // Konstruktor pro uniformní škálování (stejně na všech osách)
    Scale(float uniform);
    // Implementace virtuální metody
    glm::mat4 getMatrix() const override;
};