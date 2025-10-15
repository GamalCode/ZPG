#pragma once

class Camera;

class Controller {
private:
    Camera* camera;
    bool keys[348];          // GLFW_KEY_LAST = 348
    double lastMouseX;
    double lastMouseY;
    bool mousePressed;
public:
    Controller(Camera* cam);
    ~Controller();

    bool isKeyPressed(int key) const;
    void onKeyPress(int key);
    void onKeyRelease(int key);
    void processInput(float deltaTime);

    void onMouseMove(double x, double y);
    void onMouseButton(int button, int action);
};