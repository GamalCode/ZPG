#include "Application.h"
#include <stdio.h>
#include <stdlib.h>
#include "Transform.h"
#include "Rotation.h"
#include "Scale.h"
#include "Translation.h"
#include "tree.h"
#include "suzi_flat.h"
#include "suzi_smooth.h"
#include "sphere.h"
#include "plain.h"
#include "gift.h"
#include "bushes.h"
#include "DrawableObject.h"

Application* Application::instance = nullptr;

Application::Application() : mainWindow(nullptr), camera(nullptr), controller(nullptr), currentSceneIndex(0), deltaTime(0.0f), lastFrameTime(0.0) {
    instance = this;
}

Application::~Application() {
    for (auto shader : shaderPrograms) delete shader;
    for (auto model : modelList) delete model;
    for (auto scene : scenes) delete scene;

    if (camera) delete camera;
    if (controller) delete controller;
    if (mainLight) delete mainLight;

    if (mainWindow) {
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
    }
}

void Application::initialization() {
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        fprintf(stderr, "Can not start GLFW3\n");
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    mainWindow = glfwCreateWindow(800, 600, "OpenGL Application", NULL, NULL);
    if (!mainWindow) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(mainWindow);
    glfwSwapInterval(1);

    // Set callbacks
    glfwSetKeyCallback(mainWindow, key_callback);
    glfwSetCursorPosCallback(mainWindow, cursor_callback);
    glfwSetMouseButtonCallback(mainWindow, button_callback);
    glfwSetWindowFocusCallback(mainWindow, window_focus_callback);
    glfwSetWindowIconifyCallback(mainWindow, window_iconify_callback);
    glfwSetWindowSizeCallback(mainWindow, window_size_callback);

    glewExperimental = GL_TRUE;
    glewInit();

    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
    printf("Using GLEW %s\n", glewGetString(GLEW_VERSION));
    printf("Vendor %s\n", glGetString(GL_VENDOR));
    printf("Renderer %s\n", glGetString(GL_RENDERER));
    printf("GLSL %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    int width, height;
    glfwGetFramebufferSize(mainWindow, &width, &height);
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST); // Pro 3D objekty

    // Vytvoření kamery a controlleru
    camera = new Camera();
    controller = new Controller(camera);
    camera->setProjectionMatrix(static_cast<float>(width), static_cast<float>(height));

    lastFrameTime = glfwGetTime();

    mainLight = new Light(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    printf("Camera initialized\n");
    printf("Camera initialized\n");
    printf("Controls: WSAD = movement, Right Mouse Button + Move = look around\n");
    printf("Keys 1-7: Switch scenes, F1-F4 a G: Switch shaders\n");
}

void Application::createShaders() {
    ShaderProgram* originalShader = new ShaderProgram();
    ShaderProgram* constantShader = new ShaderProgram();
    ShaderProgram* lambertShader = new ShaderProgram();
    ShaderProgram* phongShader = new ShaderProgram();
    ShaderProgram* blinnShader = new ShaderProgram();

    // F1: PŮVODNÍ SHADER
    const char* original_vertex_shader =
        "#version 330 core\n"
        "layout(location=0) in vec3 vp;"
        "layout(location=1) in vec3 color;"
        "uniform mat4 modelMatrix;"
        "uniform mat4 viewMatrix;"
        "uniform mat4 projectionMatrix;"
        "out vec3 vertexColor;"
        "void main() {"
        "    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vp, 1.0);"
        "    vertexColor = color;"
        "}";

    const char* original_fragment_shader =
        "#version 330\n"
        "in vec3 vertexColor;"
        "out vec4 fragColor;"
        "void main() { fragColor = vec4(vertexColor,1.0); }";

    originalShader->loadMainShader(original_vertex_shader, original_fragment_shader);
    originalShader->setCamera(camera);
    addShader(originalShader);

    // F2: CONSTANT SHADER
    if (!constantShader->loadShaderFromFiles("phong.vert", "constant.frag")) {
        printf("Loading constant shader from files failed, using hardcoded version\n");
        // Fallback
        const char* vertex_shader =
            "#version 330 core\n"
            "layout(location=0) in vec3 vp;"
            "layout(location=1) in vec3 vn;"
            "uniform mat4 modelMatrix;"
            "uniform mat4 viewMatrix;"
            "uniform mat4 projectionMatrix;"
            "out vec3 worldPosition;"
            "out vec3 worldNormal;"
            "void main() {"
            "    vec4 wp = modelMatrix * vec4(vp, 1.0);"
            "    worldPosition = wp.xyz;"
            "    worldNormal = mat3(transpose(inverse(modelMatrix))) * vn;"
            "    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vp, 1.0);"
            "}";

        const char* fragment_shader =
            "#version 330\n"
            "in vec3 worldPosition;"
            "in vec3 worldNormal;"
            "out vec4 fragColor;"
            "void main() { fragColor = vec4(0.385, 0.647, 0.812, 1.0); }";

        constantShader->loadMainShader(vertex_shader, fragment_shader);
    }
    constantShader->setCamera(camera);
    addShader(constantShader);

    // F3: LAMBERT SHADER
    if (!lambertShader->loadShaderFromFiles("phong.vert", "lambert.frag")) {
        printf("Loading Lambert shader from files failed, using hardcoded version\n");
        const char* vertex_shader =
            "#version 330 core\n"
            "layout(location=0) in vec3 vp;"
            "layout(location=1) in vec3 vn;"
            "uniform mat4 modelMatrix;"
            "uniform mat4 viewMatrix;"
            "uniform mat4 projectionMatrix;"
            "out vec3 worldPosition;"
            "out vec3 worldNormal;"
            "void main() {"
            "    vec4 wp = modelMatrix * vec4(vp, 1.0);"
            "    worldPosition = wp.xyz;"
            "    worldNormal = mat3(transpose(inverse(modelMatrix))) * vn;"
            "    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vp, 1.0);"
            "}";

        const char* fragment_shader =
            "#version 330\n"
            "in vec3 worldPosition;"
            "in vec3 worldNormal;"
            "uniform vec3 lightPosition;"
            "out vec4 fragColor;"
            "void main() {"
            "    vec4 objectColor = vec4(0.385, 0.647, 0.812, 1.0);"
            "    vec4 ambient = vec4(0.1, 0.1, 0.1, 1.0);"
            "    vec3 norm = normalize(worldNormal);"
            "    vec3 lightDir = normalize(lightPosition - worldPosition);"
            "    float diff = max(dot(norm, lightDir), 0.0);"
            "    vec4 diffuse = diff * objectColor;"
            "    fragColor = ambient + diffuse;"
            "}";

        lambertShader->loadMainShader(vertex_shader, fragment_shader);
    }
    lambertShader->setCamera(camera);
    addShader(lambertShader);

    // F4: PHONG SHADER
    if (!phongShader->loadShaderFromFiles("phong.vert", "phong.frag")) {
        printf("Loading Phong shader from files failed, using hardcoded version\n");
        const char* vertex_shader =
            "#version 330 core\n"
            "layout(location=0) in vec3 vp;"
            "layout(location=1) in vec3 vn;"
            "uniform mat4 modelMatrix;"
            "uniform mat4 viewMatrix;"
            "uniform mat4 projectionMatrix;"
            "out vec3 worldPosition;"
            "out vec3 worldNormal;"
            "void main() {"
            "    vec4 wp = modelMatrix * vec4(vp, 1.0);"
            "    worldPosition = wp.xyz;"
            "    worldNormal = mat3(transpose(inverse(modelMatrix))) * vn;"
            "    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vp, 1.0);"
            "}";

        const char* fragment_shader =
            "#version 330\n"
            "in vec3 worldPosition;"
            "in vec3 worldNormal;"
            "uniform vec3 lightPosition;"
            "uniform vec3 cameraPosition;"
            "out vec4 fragColor;"
            "void main() {"
            "    vec4 objectColor = vec4(0.385, 0.647, 0.812, 1.0);"
            "    vec4 ambient = vec4(0.1, 0.1, 0.1, 1.0);"
            "    vec3 norm = normalize(worldNormal);"
            "    vec3 lightDir = normalize(lightPosition - worldPosition);"
            "    float diff = max(dot(norm, lightDir), 0.0);"
            "    vec4 diffuse = diff * objectColor;"
            "    vec3 viewDir = normalize(cameraPosition - worldPosition);"
            "    vec3 reflectDir = reflect(-lightDir, norm);"
            "    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);"
            "    vec4 specular = spec * vec4(1.0, 1.0, 1.0, 1.0);"
            "    fragColor = ambient + diffuse + specular;"
            "}";

        phongShader->loadMainShader(vertex_shader, fragment_shader);
    }
    phongShader->setCamera(camera);
    addShader(phongShader);

    // F6: BLINN-PHONG SHADER
    if (!blinnShader->loadShaderFromFiles("phong.vert", "blinn.frag")) {
        printf("Loading Blinn-Phong shader from files failed\n");
        const char* vertex_shader =
            "#version 330 core\n"
            "layout(location=0) in vec3 vp;"
            "layout(location=1) in vec3 vn;"
            "uniform mat4 modelMatrix;"
            "uniform mat4 viewMatrix;"
            "uniform mat4 projectionMatrix;"
            "out vec3 worldPosition;"
            "out vec3 worldNormal;"
            "void main() {"
            "    vec4 wp = modelMatrix * vec4(vp, 1.0);"
            "    worldPosition = wp.xyz;"
            "    worldNormal = mat3(transpose(inverse(modelMatrix))) * vn;"
            "    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vp, 1.0);"
            "}";

        const char* fragment_shader =
            "#version 330\n"
            "in vec3 worldPosition;"
            "in vec3 worldNormal;"
            "uniform vec3 lightPosition;"
            "uniform vec3 cameraPosition;"
            "out vec4 fragColor;"
            "void main() {"
            "    vec4 objectColor = vec4(0.385, 0.647, 0.812, 1.0);"
            "    vec4 ambient = vec4(0.1, 0.1, 0.1, 1.0);"
            "    vec3 norm = normalize(worldNormal);"
            "    vec3 lightDir = normalize(lightPosition - worldPosition);"
            "    float diff = max(dot(norm, lightDir), 0.0);"
            "    vec4 diffuse = diff * objectColor;"
            "    vec3 viewDir = normalize(cameraPosition - worldPosition);"
            "    vec3 halfDir = normalize(lightDir + viewDir);"
            "    float spec = pow(max(dot(norm, halfDir), 0.0), 32.0);"
            "    vec4 specular = spec * vec4(1.0, 1.0, 1.0, 1.0);"
            "    fragColor = ambient + diffuse + specular;"
            "}";

        blinnShader->loadMainShader(vertex_shader, fragment_shader);
    }
    blinnShader->setCamera(camera);
    addShader(blinnShader);
}

void Application::setupCamera() {
    // Nastavení kamery do shader programu
    if (camera && !shaderPrograms.empty()) {
        shaderPrograms[0]->setCamera(camera);
        printf("Camera connected to shader\n");
    }
}

void Application::createModels() {
    float triangle[] = {
         0.0f,  0.5f, 0.0f,   0.0f, 1.0f, 0.0f,
         0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 1.0f
    };
    addModel(new Model(triangle, 18));

    float square[] = {
         0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,
         0.9f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,
         0.9f,  0.9f, 0.0f,   1.0f, 1.0f, 0.0f,
         0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,
         0.9f,  0.9f, 0.0f,   1.0f, 1.0f, 0.0f,
         0.5f,  0.9f, 0.0f,   1.0f, 1.0f, 0.0f
    };
    addModel(new Model(square, 36));

    float rectangle[] = {
        -0.9f,  0.1f, 0.0f,   1.0f, 0.0f, 1.0f,
        -0.3f,  0.1f, 0.0f,   0.0f, 1.0f, 1.0f,
        -0.3f,  0.3f, 0.0f,   0.0f, 1.0f, 0.0f,
        -0.9f,  0.1f, 0.0f,   1.0f, 0.0f, 1.0f,
        -0.3f,  0.3f, 0.0f,   0.0f, 1.0f, 0.0f,
        -0.9f,  0.3f, 0.0f,   1.0f, 1.0f, 0.0f
    };
    addModel(new Model(rectangle, 36));

    float anotherSquare[] = {
         -1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,
         -0.6f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,
         -0.6f,  0.6f, 0.0f,   1.0f, 0.0f, 0.0f,
         -1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,
         -0.6f,  0.6f, 0.0f,   1.0f, 0.0f, 0.0f,
         -1.0f,  0.6f, 0.0f,   1.0f, 0.0f, 0.0f
    };
    addModel(new Model(anotherSquare, 36));

    const int segments = 20;
    vector<float> ballVertices;
    for (int i = 0; i < segments; i++) {
        float theta1 = i * 2.0f * 3.1415926f / segments;
        float theta2 = (i + 1) * 2.0f * 3.1415926f / segments;

        // střed
        ballVertices.push_back(0.0f); ballVertices.push_back(0.0f); ballVertices.push_back(0.0f);
        ballVertices.push_back(1.0f); ballVertices.push_back(0.0f); ballVertices.push_back(0.0f);

        // první bod na kruhu
        ballVertices.push_back(0.1f * cos(theta1));
        ballVertices.push_back(0.1f * sin(theta1));
        ballVertices.push_back(0.0f);
        ballVertices.push_back(0.0f); ballVertices.push_back(1.0f); ballVertices.push_back(0.0f);

        // druhý bod na kruhu
        ballVertices.push_back(0.1f * cos(theta2));
        ballVertices.push_back(0.1f * sin(theta2));
        ballVertices.push_back(0.0f);
        ballVertices.push_back(0.0f); ballVertices.push_back(0.0f); ballVertices.push_back(1.0f);
    }
    addModel(new Model(ballVertices.data(), ballVertices.size()));

    addModel(new Model((float*)plain, 36));
    addModel(new Model((float*)sphere, 17280));
    addModel(new Model((float*)suziFlat, 17424));
    addModel(new Model((float*)suziSmooth, 17424));
    addModel(new Model((float*)tree, 92814));
    addModel(new Model((float*)gift, 66624));
    addModel(new Model((float*)bushes, 8730));
}

void Application::createScenes() {
    // Rotující objekty
    Scene* scene1 = new Scene("Rotating Objects");

    // TRIANGLE s rotací
    Transform* t1 = new Transform();
    t1->addTransform(new Rotation(0.0f, glm::vec3(0.0f, 0.0f, 1.0f)));
    DrawableObject* tri1 = new DrawableObject(modelList[0], shaderPrograms[0], t1);
    scene1->addObject(tri1);


    // SQUARE s rotací a škálováním
    Transform* t2 = new Transform();
    t2->addTransform(new Rotation(0.0f, glm::vec3(0.0f, 0.0f, 1.0f)));
    t2->addTransform(new Scale(0.8f));
    DrawableObject* sq1 = new DrawableObject(modelList[1], shaderPrograms[0], t2);
    scene1->addObject(sq1);

    addScene(scene1);

    // Statické objekty
    Scene* scene2 = new Scene("Static Positions");

    // TRIANGLE
    Transform* s2t1 = new Transform();
    s2t1->addTransform(new Translation(-0.5f, 0.5f, 0.0f));
    s2t1->addTransform(new Scale(0.5f));
    DrawableObject* tri2 = new DrawableObject(modelList[0], shaderPrograms[0], s2t1);
    scene2->addObject(tri2);

    // SQUARE
    Transform* s2t2 = new Transform();
    s2t2->addTransform(new Translation(0.5f, 0.5f, 0.0f));
    s2t2->addTransform(new Scale(0.5f));
    DrawableObject* sq2 = new DrawableObject(modelList[1], shaderPrograms[0], s2t2);
    scene2->addObject(sq2);

    // RECTANGLE
    Transform* s2t3 = new Transform();
    s2t3->addTransform(new Translation(-0.5f, -0.5f, 0.0f));
    s2t3->addTransform(new Scale(0.5f));
    DrawableObject* rect1 = new DrawableObject(modelList[2], shaderPrograms[0], s2t3);
    scene2->addObject(rect1);

    // SQUARE
    Transform* s2t4 = new Transform();
    s2t4->addTransform(new Translation(0.5f, -0.5f, 0.0f));
    s2t4->addTransform(new Scale(0.5f));
    DrawableObject* sq3 = new DrawableObject(modelList[3], shaderPrograms[0], s2t4);
    scene2->addObject(sq3);

    addScene(scene2);

    // Komplexní transformace
    Scene* scene3 = new Scene("Complex Transforms");

    // Rotující a posunující TRIANGLE
    Transform* s3t1 = new Transform();
    s3t1->addTransform(new Translation(-0.6f, 0.2f, 0.0f));
    s3t1->addTransform(new Rotation(0.0f, glm::vec3(0.0f, 0.0f, 1.0f)));
    s3t1->addTransform(new Scale(0.5f));
    DrawableObject* tri3 = new DrawableObject(modelList[0], shaderPrograms[0], s3t1);
    scene3->addObject(tri3);

    // Vnořená transformace SQUARE
    Transform* inner = new Transform();
    inner->addTransform(new Rotation(0.0f, glm::vec3(0.0f, 0.0f, 1.0f)));
    inner->addTransform(new Scale(0.6f));

    Transform* outer = new Transform();
    outer->addTransform(new Translation(0.6f, -0.3f, 0.0f));
    outer->addTransform(inner);
    DrawableObject* sq4 = new DrawableObject(modelList[1], shaderPrograms[0], outer);
    scene3->addObject(sq4);

    addScene(scene3);

    // ROTATED TRIANGLE
    Scene* rotatingTriangleScene = new Scene("Rotated Triangle");

    Transform* triangleTransform = new Transform();
    // Nulový úhel, úhel se mění v run()
    triangleTransform->addTransform(new Rotation(0.0f, glm::vec3(0.0f, 0.0f, 1.0f)));
    DrawableObject* rotTri = new DrawableObject(modelList[0], shaderPrograms[0], triangleTransform);
    rotatingTriangleScene->addObject(rotTri);

    addScene(rotatingTriangleScene);

    // CIRCLE
    Scene* ballScene = new Scene("Four Balls");

    int ballModelIndex = 4; // index

    // levá
    Transform* b1 = new Transform();
    b1->addTransform(new Translation(-0.5f, 0.0f, 0.0f));
    DrawableObject* ball1 = new DrawableObject(modelList[4], shaderPrograms[0], b1);
    ballScene->addObject(ball1);

    // pravá
    Transform* b2 = new Transform();
    b2->addTransform(new Translation(0.5f, 0.0f, 0.0f));
    DrawableObject* ball2 = new DrawableObject(modelList[4], shaderPrograms[0], b2);
    ballScene->addObject(ball2);

    // horní
    Transform* b3 = new Transform();
    b3->addTransform(new Translation(0.0f, 0.5f, 0.0f));
    DrawableObject* ball3 = new DrawableObject(modelList[4], shaderPrograms[0], b3);
    ballScene->addObject(ball3);

    // dolní
    Transform* b4 = new Transform();
    b4->addTransform(new Translation(0.0f, -0.5f, 0.0f));
    DrawableObject* ball4 = new DrawableObject(modelList[4], shaderPrograms[0], b4);
    ballScene->addObject(ball4);

    addScene(ballScene);

    Scene* scene6 = new Scene("Complex Scene with 20 Objects");

    // Terén
    Transform* terrain = new Transform();
    terrain->addTransform(new Translation(0.0f, -0.8f, 0.0f));
    terrain->addTransform(new Scale(2.0f, 0.1f, 2.0f));
    DrawableObject* terrainObj = new DrawableObject(modelList[5], shaderPrograms[0], terrain);
    scene6->addObject(terrainObj);

    // 4 stromy v rohu
    glm::vec3 treePositions[] = {
        glm::vec3(-1.5f, -0.5f, 0.0f),
        glm::vec3(1.5f, -0.5f, 0.0f),
        glm::vec3(0.0f, -0.5f, -1.5f),
        glm::vec3(0.0f, -0.5f, 1.5f)
    };

    for (int i = 0; i < 4; i++) {
        Transform* tree = new Transform();
        tree->addTransform(new Translation(treePositions[i].x, treePositions[i].y, treePositions[i].z));
        tree->addTransform(new Scale(0.3f));
        DrawableObject* treeObj = new DrawableObject(modelList[9], shaderPrograms[0], tree);
        scene6->addObject(treeObj);
    }

    // 4 koule v rohu
    glm::vec3 spherePositions[] = {
        glm::vec3(-0.8f, -0.3f, -0.8f),
        glm::vec3(0.8f, -0.3f, -0.8f),
        glm::vec3(-0.8f, -0.3f, 0.8f),
        glm::vec3(0.8f, -0.3f, 0.8f)
    };

    for (int i = 0; i < 4; i++) {
        Transform* sphere = new Transform();
        sphere->addTransform(new Translation(spherePositions[i].x, spherePositions[i].y, spherePositions[i].z));
        sphere->addTransform(new Scale(0.2f));
        DrawableObject* sphereObj = new DrawableObject(modelList[6], shaderPrograms[0], sphere);
        scene6->addObject(sphereObj);
    }

    // 2 suzi flat s rotací
    float suziRotations[] = { 45.0f, -45.0f };
    for (int i = 0; i < 2; i++) {
        Transform* suziFlat = new Transform();
        suziFlat->addTransform(new Translation(-1.0f + i * 2.0f, -0.5f, 0.0f));
        suziFlat->addTransform(new Rotation(glm::radians(suziRotations[i]), glm::vec3(0.0f, 1.0f, 0.0f)));
        suziFlat->addTransform(new Scale(0.4f));
        DrawableObject* suziObj = new DrawableObject(modelList[7], shaderPrograms[0], suziFlat);
        scene6->addObject(suziObj);
    }

    // 2 suzi smooth s rotací
    float suziSmoothRotations[] = { 90.0f, -90.0f };
    for (int i = 0; i < 2; i++) {
        Transform* suziSmooth = new Transform();
        suziSmooth->addTransform(new Translation(0.0f, -0.5f, -1.0f + i * 2.0f));
        suziSmooth->addTransform(new Rotation(glm::radians(suziSmoothRotations[i]), glm::vec3(0.0f, 1.0f, 0.0f)));
        suziSmooth->addTransform(new Scale(0.4f));
        DrawableObject* suziSmoothObj = new DrawableObject(modelList[8], shaderPrograms[0], suziSmooth);
        scene6->addObject(suziSmoothObj);
    }

    // 4 dárky v rohu
    glm::vec3 giftPositions[] = {
        glm::vec3(-0.5f, -0.5f, -0.5f),
        glm::vec3(0.5f, -0.5f, -0.5f),
        glm::vec3(-0.5f, -0.5f, 0.5f),
        glm::vec3(0.5f, -0.5f, 0.5f)
    };

    for (int i = 0; i < 4; i++) {
        Transform* gift = new Transform();
        gift->addTransform(new Translation(giftPositions[i].x, giftPositions[i].y, giftPositions[i].z));
        gift->addTransform(new Scale(0.2f));
        DrawableObject* giftObj = new DrawableObject(modelList[10], shaderPrograms[0], gift);
        scene6->addObject(giftObj);
    }

    // 3 křoviny
    glm::vec3 bushPositions[] = {
        glm::vec3(-1.2f, -0.5f, 1.2f),
        glm::vec3(1.2f, -0.5f, 1.2f),
        glm::vec3(0.0f, -0.5f, 1.5f)
    };

    for (int i = 0; i < 3; i++) {
        Transform* bush = new Transform();
        bush->addTransform(new Translation(bushPositions[i].x, bushPositions[i].y, bushPositions[i].z));
        bush->addTransform(new Scale(0.25f));
        DrawableObject* bushObj = new DrawableObject(modelList[11], shaderPrograms[0], bush);
        scene6->addObject(bushObj);
    }

    addScene(scene6);

    Scene* scene7 = new Scene("Forest - 50 Trees & 50 Bushes");

    srand(42);

    Transform* forestTerrain = new Transform();
    forestTerrain->addTransform(new Translation(0.0f, -1.0f, 0.0f));
    forestTerrain->addTransform(new Scale(50.0f, 0.5f, 50.0f));

    DrawableObject* forestTerrainObj = new DrawableObject(modelList[5], shaderPrograms[0], forestTerrain);
    scene7->addObject(forestTerrainObj);

    for (int i = 0; i < 50; i++) {
        // (-20 až 20) x (-20 až 20)
        float posX = (rand() % 4000) / 100.0f - 20.0f;
        float posZ = (rand() % 4000) / 100.0f - 20.0f;

        // Osa Y
        float rotationY = (rand() % 360) * 3.14159f / 180.0f;

        // velikost (0.2 až 0.4)
        float scale = 0.2f + (rand() % 20) / 100.0f;

        Transform* tree = new Transform();
        tree->addTransform(new Translation(posX, -0.5f, posZ));
        tree->addTransform(new Rotation(rotationY, glm::vec3(0.0f, 1.0f, 0.0f)));
        tree->addTransform(new Scale(scale));

        DrawableObject* treeObj = new DrawableObject(modelList[9], shaderPrograms[0], tree);
        scene7->addObject(treeObj);
    }

    for (int i = 0; i < 50; i++) {
        // (-20 až 20) x (-20 až 20)
        float posX = (rand() % 4000) / 100.0f - 20.0f;
        float posZ = (rand() % 4000) / 100.0f - 20.0f;

        // Osa Y
        float rotationY = (rand() % 360) * 3.14159f / 180.0f;

        // velikost (0.15 až 0.35)
        float scale = 0.15f + (rand() % 20) / 100.0f;

        Transform* bush = new Transform();
        bush->addTransform(new Translation(posX, -0.5f, posZ));
        bush->addTransform(new Rotation(rotationY, glm::vec3(0.0f, 1.0f, 0.0f)));
        bush->addTransform(new Scale(scale));

        DrawableObject* bushObj = new DrawableObject(modelList[11], shaderPrograms[0], bush);
        scene7->addObject(bushObj);
    }

    addScene(scene7);

    Scene* testScene1 = new Scene("Test Scene 1 - Four Spheres");

    // Přidaní malé žluté koule uprostřed pro světlo
    Transform* lightVis = new Transform();
    lightVis->addTransform(new Scale(0.1f));
    DrawableObject* lightSphere = new DrawableObject(modelList[6], shaderPrograms[0], lightVis);
    testScene1->addObject(lightSphere);

    glm::vec3 testSpherePositions[] = {
        glm::vec3(-0.7f, 0.0f, 0.0f),   // levá
        glm::vec3(0.7f, 0.0f, 0.0f),    // pravá  
        glm::vec3(0.0f, 0.7f, 0.0f),    // horní
        glm::vec3(0.0f, -0.7f, 0.0f)    // dolní
    };

    for (int i = 0; i < 4; i++) {
        Transform* sphereTransform = new Transform();
        sphereTransform->addTransform(new Translation(testSpherePositions[i]));
        sphereTransform->addTransform(new Scale(0.3f));

        DrawableObject* sphere = new DrawableObject(modelList[6], shaderPrograms[0], sphereTransform);
        testScene1->addObject(sphere);
    }

    addScene(testScene1);

    printf("Created %d scenes\n", getSceneCount());
}

void Application::run() {
    float angle = 0.0f;
    glm::vec3 lightPosition(0.0f, 0.0f, 0.0f); // Světlo na pozici [0,0,0]

    while (!glfwWindowShouldClose(mainWindow)) {
        // Výpočet delta time
        double currentFrame = glfwGetTime();
        deltaTime = static_cast<float>(currentFrame - lastFrameTime);
        lastFrameTime = currentFrame;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        angle += 0.01f; // rotace

        // Zpracování vstupu
        if (controller) {
            controller->processInput(deltaTime);
        }

        // Vykreslení aktuální scény
        if (currentSceneIndex >= 0 && currentSceneIndex < getSceneCount()) {
            Scene* currentScene = scenes[currentSceneIndex];

            for (const auto& drawable : currentScene->getObjects()) {
                // Nastavení shader matric
                if (camera && drawable->getShader()) {
                    ShaderProgram* shader = drawable->getShader();
                    shader->use(shader->getProgram());

                    // Nastav matice
                    shader->SetUniform("viewMatrix", camera->getViewMatrix());
                    shader->SetUniform("projectionMatrix", camera->getProjectionMatrix());

                    // Nastav lighting uniformy (pro Lambert a Phong shadery)
                    shader->SetUniform("lightPosition", lightPosition);
                    shader->SetUniform("cameraPosition", camera->getEye());

                    if (currentSceneIndex == 3) {
                        // Speciální pro scénu 3
                        Transform* dynamicTransform = new Transform();
                        dynamicTransform->addTransform(new Rotation(angle, glm::vec3(0.0f, 0.0f, 1.0f)));
                        drawable->setTransform(dynamicTransform);
                        drawable->draw();
                        delete dynamicTransform;
                    }
                    else {
                        // Ostatní
                        drawable->draw();
                    }
                }
            }
        }

        glfwPollEvents();
        glfwSwapBuffers(mainWindow);
    }
}

void Application::addShader(ShaderProgram* shader) {
    shaderPrograms.push_back(shader);
}

void Application::addModel(Model* model) {
    modelList.push_back(model);
}

void Application::addScene(Scene* scene) {
    scenes.push_back(scene);
}

void Application::switchScene(int sceneIndex) {
    if (sceneIndex >= 0 && sceneIndex < getSceneCount()) {
        currentSceneIndex = sceneIndex;
        printf("Switched to scene %d: %s\n", sceneIndex + 1,
            scenes[sceneIndex]->getName().c_str());
    }
}

void Application::switchShader(int shaderIndex) {
    if (shaderIndex >= 0 && shaderIndex < getShaderCount()) {
        // Změň shader pro všechny objekty
        if (currentSceneIndex >= 0 && currentSceneIndex < getSceneCount()) {
            Scene* currentScene = scenes[currentSceneIndex];
            for (auto& drawable : currentScene->getObjects()) {
                drawable->setShader(shaderPrograms[shaderIndex]);
            }
            const char* shaderNames[] = { "Original Colors", "Constant", "Lambert", "Phong", "Blinn"};
            printf("Switched to %s shader\n", shaderNames[shaderIndex]);
        }
    }
}

int Application::getModelCount() const {
    return static_cast<int>(modelList.size());
}

int Application::getShaderCount() const {
    return static_cast<int>(shaderPrograms.size());
}

int Application::getSceneCount() const {
    return static_cast<int>(scenes.size());
}

// Callback Functions
void Application::error_callback(int error, const char* description) {
    fputs(description, stderr);
}

void Application::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    // Přepínání scén pomocí číslic 1–6
    if (instance != nullptr) {
        if (instance->controller) {
            if (action == GLFW_PRESS) {
                instance->controller->onKeyPress(key);
            }
            else if (action == GLFW_RELEASE) {
                instance->controller->onKeyRelease(key);
            }
        }

        // Přepínání scén pomocí číslic 1–8
        if (action == GLFW_PRESS) {
            if (key == GLFW_KEY_1) instance->switchScene(0);
            else if (key == GLFW_KEY_2) instance->switchScene(1);
            else if (key == GLFW_KEY_3) instance->switchScene(2);
            else if (key == GLFW_KEY_4) instance->switchScene(3);
            else if (key == GLFW_KEY_5) instance->switchScene(4);
            else if (key == GLFW_KEY_6) instance->switchScene(5);
            else if (key == GLFW_KEY_7) instance->switchScene(6);
            else if (key == GLFW_KEY_8) instance->switchScene(7);
            // Přepínání shaderů pomocí F1-F4 a G
            else if (key == GLFW_KEY_F1) instance->switchShader(0);
            else if (key == GLFW_KEY_F2) instance->switchShader(1);
            else if (key == GLFW_KEY_F3) instance->switchShader(2);
            else if (key == GLFW_KEY_F4) instance->switchShader(3);
            else if (key == GLFW_KEY_G) instance->switchShader(4);
        }
    }
}

void Application::window_focus_callback(GLFWwindow* window, int focused) {}
void Application::window_iconify_callback(GLFWwindow* window, int iconified) {}

void Application::window_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    if (instance != nullptr && instance->camera) {
        instance->camera->setProjectionMatrix(static_cast<float>(width), static_cast<float>(height));
    }
}

void Application::cursor_callback(GLFWwindow* window, double x, double y) {
    if (instance != nullptr && instance->controller) {
        instance->controller->onMouseMove(x, y);
    }
}

void Application::button_callback(GLFWwindow* window, int button, int action, int mode) {
    if (instance != nullptr && instance->controller) {
        instance->controller->onMouseButton(button, action);
    }
}
