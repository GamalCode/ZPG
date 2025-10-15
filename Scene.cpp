#include "Scene.h"

Scene::Scene(const string& sceneName) : name(sceneName) {}

Scene::~Scene() {
    for (auto obj : objects) {
        delete obj;
    }
    objects.clear();
}

void Scene::addObject(DrawableObject* drawable) {
    objects.push_back(drawable);
}

const vector<DrawableObject*>& Scene::getObjects() const {
    return objects;
}

const string& Scene::getName() const {
    return name;
}

int Scene::getObjectCount() const {
    return static_cast<int>(objects.size());
}