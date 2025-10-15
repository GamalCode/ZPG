#pragma once
#include <GL/glew.h>

class Model {
private:
    GLuint VAO;
    GLuint VBO;
    int numberOfVertices;
public:
    Model(float* vertices, int numberOfFloats); // Vytvoří model
    ~Model();

    void draw();          // Vykreslí model
    GLuint getVAO() const { return VAO; } // pokud bude potřeba VAO externě
    int getNumberOfVertices() const { return numberOfVertices; }
};