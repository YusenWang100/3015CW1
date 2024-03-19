#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>
#include "helper/glslprogram.h"
#include "helper/scene.h"
#include "scene0/camera.h"
#include "scene0/mesh.h"
#include "scene0/skybox.h"

struct Light {
    struct UBOData {
        // point light
        glm::vec4 lightPosition;
        glm::vec4 lightColor;
        // moving Light
        glm::vec4 movingLightPosition;
        glm::vec4 movingLightColor;
        // directional light
        glm::vec4 directionalLightColor;

        // ambient light
        glm::vec4 ambientColor;
            //spot light
        glm::vec3 spotLightPosition0;
        float spotLightUmbra;  // rad
        glm::vec3 spotLightDirection0;
        float spotLightPenumbra;  // rad
        glm::vec3 spotLightColor;
        float pad0;
        glm::vec3 spotLightPosition1;
        float pad1;
        glm::vec3 spotLightDirection1;
    } uboData;
    GLuint ubo;

    void Init();
    void Update();
    void Bind(int index);
};

class SceneBasic_Uniform : public Scene {
private:
    int width;
    int height;

    GLSLProgram combineProgram;
    GLSLProgram blurProgram;
    GLSLProgram defaultProgram;
    GLSLProgram skyboxProgram;

    GLuint fbo{};
    Texture colorTex0;
    Texture colorTex1;
    Texture depthTex;

    GLuint blurFbos[2]{};
    Texture blurColorTexs[2];

    Camera camera{};

    Light light;
    Skybox skybox;

    Model house;
    Model ground;
    Model car;

    Model tree;
    Model trunk;
    Model table;

    Model sphere;

    Model dragon;

    Model crate;

    Model dog;
    Model wooden;
    Model plants;

    Model signature;

    Mesh triangle;

    float dogOffset = 20.f;
    float headHorizontalAngle = 0.0f;
    float headVerticalAngle = 10.0f;
    float tailHorizontalAngle = 0.0f;
    float tailVerticalAngle = -10.0f;
    float tailWiggleAngle = 0.0f;
    bool tailWiggleDirectionLeft = true;
    float legsAngle = 0.0f;
    bool legsMovementDirectionForward = true;
    bool isMoving = false;

    glm::vec2 lastMousePosition;
    bool rightMouseButtonDown = false;
    float movingLightRotation = 1.0f;
    bool pauseAnimation = false;
    bool firstEnter = false;

    float treeRotation = 0.0f;
    float treeRotationDirection = 1.0f;

    float tSum = 0;

    void createFBO();
    void deleteFBO();
    void createProgram(GLSLProgram& program, size_t count, const char* const* files);
    void compile();
    void drawTree(glm::vec3 const& pos, glm::vec3 const& scale);
    void drawTable(glm::vec3 const& pos, glm::vec3 const& scale);
    void drawDog();
    void drawDragon();
    void drawCrate();

public:
    SceneBasic_Uniform();

    void initScene();
    void update(float t,GLFWwindow*);
    void render();
    void resize(int, int);
    void mouseMoveCallback(double xpos, double ypos) override;
    void mouseButtonCallback(int button, int action, int mods) override;
};

#endif // SCENEBASIC_UNIFORM_H
