#pragma once
#include <vector>
#include "DrawableObject.h"
#include <string>

using namespace std;

class Scene {
private:
    vector<DrawableObject*> objects;
    string name;
public:
    Scene(const string& sceneName);
    ~Scene();

    void addObject(DrawableObject* drawable);
    const vector<DrawableObject*>& getObjects() const;
    const string& getName() const;
    int getObjectCount() const;
};