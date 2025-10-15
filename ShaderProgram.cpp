#include "ShaderProgram.h"
#include "Camera.h"
#include <fstream>
#include <sstream>
#include <string>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

ShaderProgram::ShaderProgram()
    : vertexShader(0), fragmentShader(0), shaderProgram(0), m_camera(nullptr) {
}

ShaderProgram::ShaderProgram(Camera* camera)
    : vertexShader(0), fragmentShader(0), shaderProgram(0), m_camera(camera) {
    if (m_camera) {
        m_camera->attach(this);
    }
}

ShaderProgram::~ShaderProgram() {
    if (m_camera) {
        m_camera->detach(this);
    }
    if (shaderProgram) glDeleteProgram(shaderProgram);
    if (vertexShader) glDeleteShader(vertexShader);
    if (fragmentShader) glDeleteShader(fragmentShader);
}

bool ShaderProgram::compileShader(GLuint shader, const char* source) {
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        printf("Error: Compilation failed\n%s\n", infoLog);
        return false;
    }
    return true;
}

bool ShaderProgram::loadShaderFromFiles(const char* vertexPath, const char* fragmentPath) {
    // Načtení vertex shaderu ze souboru
    ifstream vertexFile(vertexPath);
    if (!vertexFile.is_open()) {
        printf("Unable to open vertex shader file: %s\n", vertexPath);
        return false;
    }
    stringstream vertexBuffer;
    vertexBuffer << vertexFile.rdbuf();
    string vertexCode = vertexBuffer.str();
    vertexFile.close();

    // Načtení fragment shaderu ze souboru
    ifstream fragmentFile(fragmentPath);
    if (!fragmentFile.is_open()) {
        printf("Unable to open fragment shader file: %s\n", fragmentPath);
        return false;
    }
    stringstream fragmentBuffer;
    fragmentBuffer << fragmentFile.rdbuf();
    string fragmentCode = fragmentBuffer.str();
    fragmentFile.close();

    // Kompilace shaderů
    return loadMainShader(vertexCode.c_str(), fragmentCode.c_str());
}

bool ShaderProgram::loadMainShader(const char* vertexSource, const char* fragmentSource) {
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    if (!compileShader(vertexShader, vertexSource)) return false;

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    if (!compileShader(fragmentShader, fragmentSource)) return false;

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint status;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infoLogLength;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
        GLchar* strInfoLog = new GLchar[infoLogLength + 1];
        glGetProgramInfoLog(shaderProgram, infoLogLength, NULL, strInfoLog);
        printf("Error: Linking failed: %s\n", strInfoLog);
        delete[] strInfoLog;
        return false;
    }
    return true;
}

void ShaderProgram::use(GLuint program) {
    glUseProgram(program);
}

GLuint ShaderProgram::getProgram() const {
    return shaderProgram;
}

void ShaderProgram::SetUniform(const char* name, float value) {
    GLint location = glGetUniformLocation(shaderProgram, name);
    if (location != -1) {
        glUniform1f(location, value);
    }
    else {
        printf("Warning: uniform '%s' not found!\n", name);
    }
}

void ShaderProgram::SetUniform(const char* name, int value) {
    GLint location = glGetUniformLocation(shaderProgram, name);
    if (location != -1) {
        glUniform1i(location, value);
    }
    else {
        printf("Warning: uniform '%s' not found!\n", name);
    }
}

void ShaderProgram::SetUniform(const char* name, const glm::vec3& value) {
    GLint location = glGetUniformLocation(shaderProgram, name);
    if (location != -1) {
        glUniform3fv(location, 1, glm::value_ptr(value));
    }
    else if (strcmp(name, "lightPosition") != 0 && strcmp(name, "cameraPosition") != 0) {
        printf("Warning: uniform '%s' not found!\n", name);
    }
}

void ShaderProgram::SetUniform(const char* name, const glm::mat4& value) {
    GLint location = glGetUniformLocation(shaderProgram, name);
    if (location != -1) {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }
    else if (strcmp(name, "viewMatrix") != 0 && strcmp(name, "projectionMatrix") != 0) {
        printf("Warning: uniform '%s' not found!\n", name);
    }
}

void ShaderProgram::notify(Camera* camera) {
    if (camera) {
        m_camera = camera;
    }
}


void ShaderProgram::setCamera(Camera* camera) {
    if (m_camera) {
        m_camera->detach(this);
    }
    m_camera = camera;
    if (m_camera) {
        m_camera->attach(this);
    }
}