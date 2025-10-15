#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>

using namespace std;

class Observer;

class Camera {
private:
    glm::vec3 eye;           // Pozice kamery
    glm::vec3 target;        // Směr pohledu
    glm::vec3 up;            // Vektor nahoru

    float alpha;             // Úhel rotace okolo X osy (pitch)
    float fi;                // Úhel rotace okolo Y osy (yaw)
    float speed;             // Rychlost pohybu
    float sensitivity;       // Citlivost myši

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    vector<Observer*> observers;

    void updateViewMatrix();
    void updateTargetVector();

    void clampAngles();
public:
    Camera();
    ~Camera();

    glm::mat4 getViewMatrix() const { return viewMatrix; }
    glm::mat4 getProjectionMatrix() const { return projectionMatrix; }

    void moveForward(float deltaTime);
    void moveBackward(float deltaTime);
    void moveLeft(float deltaTime);
    void moveRight(float deltaTime);

    // Otáčení podle myši
    void rotate(float deltaX, float deltaY);

    // Observer pattern
    void attach(Observer* observer);
    void detach(Observer* observer);
    void notify();

    // Získání pozice myši
    glm::vec3 getEye() const { return eye; }
    glm::vec3 getTarget() const { return target; }
    glm::vec3 getUp() const { return up; }

    // Nastavení
    void setProjectionMatrix(float width, float height);
};