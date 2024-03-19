#pragma once
#include <glad/glad.h>

#include <glm/ext.hpp>
#include <glm/glm.hpp>

enum Camera_Movement { FORWARD, BACKWARD, UP, DOWN, LEFT, RIGHT };

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float MINSPPED = 12.5f;
const float SPEED = 25.0f;
const float MAXSPEED = 50.0F;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

struct Camera {
    glm::vec3 Position{0, 0, 5};
    glm::vec3 WorldUp{0, 1, 0};

    glm::vec3 Front{0, 0, -1};
    glm::vec3 Up;
    glm::vec3 Right;

    float fovy{60};
    float aspect{1};
    float zNear{1};
    float zFar{1000};

    // euler Angles
    float Yaw = YAW;
    float Pitch = PITCH;

    float MovementSpeed = SPEED;
    float MouseSensitivity = SENSITIVITY;
    float Zoom = ZOOM;

    struct CameraUBO {
        glm::mat4 viewMatrix;
        glm::mat4 projectMatrix;
        glm::vec3 viewPosition;
        float fogFactor{0.01};
        glm::vec3 fogColor{1, 1, 1};
    } uboData{};

    GLuint ubo;
    void Init() {
        UpdateData();

        glGenBuffers(1, &ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraUBO), &uboData, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    void UpdateData() {
        uboData.viewMatrix = glm::lookAt(Position, Position + Front, WorldUp);
        uboData.projectMatrix = glm::perspective(glm::radians(fovy), aspect, zNear, zFar);
        uboData.viewPosition = Position;

        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(
            glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you
                                          // look up or down which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));
    }

    void Update() {
        UpdateData();
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraUBO), &uboData);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    void Bind(int index) { glBindBufferBase(GL_UNIFORM_BUFFER, index, ubo); }

    void ProcessKeyboard(Camera_Movement direction, float deltaTime) {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD) Position += Front * velocity;
        if (direction == BACKWARD) Position -= Front * velocity;
        if (direction == UP) Position += Up * velocity;
        if (direction == DOWN) Position -= Up * velocity;
        if (direction == LEFT) Position -= Right * velocity;
        if (direction == RIGHT) Position += Right * velocity;
        Update();
    }
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true) {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch) {
            if (Pitch > 89.0f) Pitch = 89.0f;
            if (Pitch < -89.0f) Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        Update();
    }
    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset) {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f) Zoom = 1.0f;
        if (Zoom > 45.0f) Zoom = 45.0f;
        Update();
    }

    void SetMinSpeed() { MovementSpeed = MINSPPED; }

    void SetNormalSpeed() { MovementSpeed = SPEED; }

    void SetMaxSpeed() { MovementSpeed = MAXSPEED; }
};