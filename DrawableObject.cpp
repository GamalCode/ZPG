#include "DrawableObject.h"
#include "Model.h"
#include "ShaderProgram.h"
#include "CompositeTransform.h"

DrawableObject::DrawableObject(Model* m, ShaderProgram* s, CompositeTransform* t)
    : model(m), shader(s), transform(t) {
}

DrawableObject::~DrawableObject() { }

void DrawableObject::draw() {
    if (!model || !shader) return;

    GLuint program = shader->getProgram();
    shader->use(program);

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    if (transform != nullptr) {
        modelMatrix = transform->getMatrix();
    }

    shader->SetUniform("modelMatrix", modelMatrix);
    model->draw();
}

void DrawableObject::setTransform(CompositeTransform* t) {
    transform = t;
}

void DrawableObject::setShader(ShaderProgram* s) {
    shader = s;
}