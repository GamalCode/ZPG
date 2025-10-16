#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "ShaderProgram.h"
#include "Model.h"
#include "CompositeTransform.h"
#include "Scene.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.h"
#include "Controller.h"
#include "Light.h"

using namespace std;

class Application {
private:
    GLFWwindow* mainWindow;
    vector<ShaderProgram*> shaderPrograms;
    vector<Model*> modelList;
    vector<Scene*> scenes;
    int currentSceneIndex;

    Camera* camera;
    Controller* controller;
    float deltaTime;
    double lastFrameTime;

    Light* mainLight;

    // Callback Functions
    static void error_callback(int error, const char* description);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void window_focus_callback(GLFWwindow* window, int focused);
    static void window_iconify_callback(GLFWwindow* window, int iconified);
    static void window_size_callback(GLFWwindow* window, int width, int height);
    static void cursor_callback(GLFWwindow* window, double x, double y);
    static void button_callback(GLFWwindow* window, int button, int action, int mode);

    static Application* instance;

public:
    Application();
    ~Application();

    void initialization();
    void createShaders();
    void createModels();
    void createScenes();
    void setupCamera();
    void run();

    void addShader(ShaderProgram* shader);
    void addModel(Model* model);
    void addScene(Scene* scene);
    void switchScene(int sceneIndex);

    int getModelCount() const;
    int getShaderCount() const;
    int getSceneCount() const;
    void switchShader(int shaderIndex); // NEW: Switch shader for current scene

    Camera* getCamera() const { return camera; }
    Controller* getController() const { return controller; }
};
