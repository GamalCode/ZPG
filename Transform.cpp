#include "Transform.h"

Transform::~Transform() {
    for (auto child : children) {
        delete child;
    }
    children.clear();
}

void Transform::addTransform(CompositeTransform* transform) {
    children.push_back(transform);
}

void Transform::removeTransform(CompositeTransform* transform) {
    for (auto it = children.begin(); it != children.end(); ++it) {
        if (*it == transform) {
            children.erase(it);
            break;
        }
    }
}

glm::mat4 Transform::getMatrix() const {
    glm::mat4 result = glm::mat4(1.0f);

    for (const auto& child : children) {
        result = result * child->getMatrix();
    }

    return result;
}