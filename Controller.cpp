#include "Controller.h"
#include "Camera.h"
#include <GLFW/glfw3.h>
#include <cstring>

Controller::Controller(Camera* cam)
    : camera(cam), lastMouseX(0.0), lastMouseY(0.0), mousePressed(false) {

    std::memset(keys, 0, sizeof(keys));
}

Controller::~Controller() { }

void Controller::onKeyPress(int key) {
    if (key >= 0 && key < 348) {
        keys[key] = true;
    }
}

void Controller::onKeyRelease(int key) {
    if (key >= 0 && key < 348) {
        keys[key] = false;
    }
}

void Controller::onMouseMove(double x, double y) {
    if (mousePressed) {
        double deltaX = x - lastMouseX;
        double deltaY = y - lastMouseY;

        if (camera) {
            camera->rotate(static_cast<float>(deltaX), static_cast<float>(deltaY));
        }
    }

    lastMouseX = x;
    lastMouseY = y;
}

void Controller::onMouseButton(int button, int action) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            mousePressed = true;
        }
        else {
            mousePressed = false;
        }
    }
}

bool Controller::isKeyPressed(int key) const {
    if (key >= 0 && key < 348) {
        return keys[key];
    }
    return false;
}

void Controller::processInput(float deltaTime) {
    if (!camera) return;

    // WSAD pohyb
    if (isKeyPressed(GLFW_KEY_W)) {
        camera->moveForward(deltaTime);
    }
    if (isKeyPressed(GLFW_KEY_S)) {
        camera->moveBackward(deltaTime);
    }
    if (isKeyPressed(GLFW_KEY_A)) {
        camera->moveLeft(deltaTime);
    }
    if (isKeyPressed(GLFW_KEY_D)) {
        camera->moveRight(deltaTime);
    }
}