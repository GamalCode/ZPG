#pragma once
#include "CompositeTransform.h"
#include <vector>

using namespace std;

class Transform : public CompositeTransform {
private:
    vector<CompositeTransform*> children;
public:
    // uvolní všechny child transformace
    ~Transform();
    // Přidání child transformace do kompozice
    void addTransform(CompositeTransform* transform);
    // Odstranění child transformace
    void removeTransform(CompositeTransform* transform);
    // Vynásobí child matice dohromady
    glm::mat4 getMatrix() const override;
};