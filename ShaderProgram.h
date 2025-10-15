#pragma once
#include <GL/glew.h>
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Observer.h"

class Camera;

class ShaderProgram : public Observer {
private:
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint shaderProgram;

    Camera* m_camera;

    bool compileShader(GLuint shader, const char* source);
public:
    ShaderProgram();
    ShaderProgram(Camera* camera);
    virtual ~ShaderProgram();

    // Shader s barvami
    bool loadMainShader(const char* vertexSource, const char* fragmentSource);

    bool loadShaderFromFiles(const char* vertexPath, const char* fragmentPath);

    void use(GLuint program);
    GLuint getProgram() const;

    // Přetížené metody SetUniform
    void SetUniform(const char* name, float value);
    void SetUniform(const char* name, int value);
    void SetUniform(const char* name, const glm::vec3& value);
    void SetUniform(const char* name, const glm::mat4& value);

    // Observer metoda
    virtual void notify(Camera* camera);
    void setCamera(Camera* camera);
};