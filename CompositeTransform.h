#pragma once
#include <glm/glm.hpp>

// Component v Composite patternu
class CompositeTransform {
public:
    virtual ~CompositeTransform();

    // Každá podtřída ji musí implementovat
    virtual glm::mat4 getMatrix() const = 0;
};