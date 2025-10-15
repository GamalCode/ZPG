#pragma once

class Camera;

class Observer {
public:
    virtual ~Observer() {}
    virtual void notify(Camera* camera) = 0;
};