#include "Camera.h"
#include "Observer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>

Camera::Camera()
    : eye(0.0f, 0.0f, 3.0f),
    target(0.0f, 0.0f, -1.0f),
    up(0.0f, 1.0f, 0.0f),
    alpha(glm::pi<float>() / 2.0f),     // 90 stupňů (pohled dolů)
    fi(3.0f * glm::pi<float>() / 2.0f), // 270° - kouká na k objektům)
    speed(5.0f),
    sensitivity(0.005f) {

    viewMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::mat4(1.0f);

    updateViewMatrix();
    setProjectionMatrix(800.0f, 600.0f);
}

Camera::~Camera() {
    observers.clear();
}

void Camera::updateTargetVector() {
    // Výpočet cílového vektoru na základě úhlů
    target.x = sin(alpha) * cos(fi);
    target.z = sin(alpha) * sin(fi);
    target.y = cos(alpha);
}

void Camera::clampAngles() {
    // Alpha omezení - zabráníme překlopení kamery
    const float minAlpha = 0.01f;
    const float maxAlpha = glm::pi<float>() - 0.01f;

    if (alpha < minAlpha) alpha = minAlpha;
    if (alpha > maxAlpha) alpha = maxAlpha;

    if (fi < 0.0f) fi += 2.0f * glm::pi<float>();
    if (fi > 2.0f * glm::pi<float>()) fi -= 2.0f * glm::pi<float>();
}

void Camera::updateViewMatrix() {
    viewMatrix = glm::lookAt(eye, eye + target, up);
}

void Camera::moveForward(float deltaTime) {
    glm::vec3 forward = glm::normalize(target);
    eye += forward * speed * deltaTime;
    updateViewMatrix();
}

void Camera::moveBackward(float deltaTime) {
    glm::vec3 forward = glm::normalize(target);
    eye -= forward * speed * deltaTime;
    updateViewMatrix();
}

void Camera::moveLeft(float deltaTime) {
    glm::vec3 forward = glm::normalize(target);
    glm::vec3 right = glm::normalize(glm::cross(forward, up));
    glm::vec3 left = -right;
    eye += left * speed * deltaTime;
    updateViewMatrix();
}

void Camera::moveRight(float deltaTime) {
    glm::vec3 forward = glm::normalize(target);
    glm::vec3 right = glm::normalize(glm::cross(forward, up));
    eye += right * speed * deltaTime;
    updateViewMatrix();
}

void Camera::rotate(float deltaX, float deltaY) {
    if (glm::abs(deltaX) < 0.1f && glm::abs(deltaY) < 0.1f) {
        return;
    }

    // Myš se pohybuje dolů = (alpha se zvětšuje)
    alpha += deltaY * sensitivity;

    // Myš se pohybuje doprava = (fi se zvětšuje)
    fi += deltaX * sensitivity;

    clampAngles();
    updateTargetVector();
    updateViewMatrix();

    notify();
}

void Camera::setProjectionMatrix(float width, float height) {
    float aspectRatio = width / height;
    projectionMatrix = glm::perspective(
        glm::radians(60.0f),  // POV
        aspectRatio,           // průměr
        0.1f,                  // poblíž
        100.0f                 // Daleko
    );

    notify();
}

void Camera::attach(Observer* observer) {
    if (observer) {
        observers.push_back(observer);
    }
}

void Camera::detach(Observer* observer) {
    auto it = std::find(observers.begin(), observers.end(), observer);
    if (it != observers.end()) {
        observers.erase(it);
    }
}

void Camera::notify() {
    for (auto observer : observers) {
        observer->notify(this);
    }
}