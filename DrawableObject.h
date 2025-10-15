#pragma once

class Model;
class ShaderProgram;
class CompositeTransform;

class DrawableObject {
private:
    Model* model;
    ShaderProgram* shader;
    CompositeTransform* transform;
public:
    DrawableObject(Model* m, ShaderProgram* s, CompositeTransform* t = nullptr);
    ~DrawableObject();

    void draw();

    Model* getModel() const { return model; }
    ShaderProgram* getShader() const { return shader; }
    CompositeTransform* getTransform() const { return transform; }

    void setTransform(CompositeTransform* t);
    void setShader(ShaderProgram* s);
};
