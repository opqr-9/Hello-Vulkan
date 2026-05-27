#pragma once
#include "Start.h"

glm::vec3 defaultCameraPos = glm::vec3(4.0f, 0.0f, 4.0f);
//const glm::vec3 cameraPos = glm::vec3(4.0f, 0.0f, 4.0f);
glm::vec3 defaultCameraDir = glm::normalize(-defaultCameraPos);
glm::vec3 defaultCameraWorldUp = glm::vec3(0.0f, 0.0f, 1.0f);
float defaultVerticalFOV = glm::radians(45.0f);

class Camera
{
public:
    glm::vec3 pos;
    glm::vec3 dir;
    glm::vec3 worldUp;
    glm::vec3 up;
    glm::vec3 left;
    float verticalFOV;
    float horizontalFOV;

    Camera()
    {
        pos = defaultCameraPos;
        dir = defaultCameraDir;
        worldUp = defaultCameraWorldUp;
        left = glm::normalize(glm::cross(worldUp, dir));
        up = glm::normalize(glm::cross(dir, left));
        verticalFOV = defaultVerticalFOV;
        horizontalFOV = 2 * glm::atan(windowSize.width / 2 / (windowSize.height / 2 / glm::tan(verticalFOV / 2)));
    }

    void MoveForward(float deltaTime)
    {
        pos += glm::normalize(dir) * deltaTime;
    }

    void MoveBackward(float deltaTime)
    {
        pos -= glm::normalize(dir) * deltaTime;
    }

    void MoveLeft(float deltaTime)
    {
        pos += glm::normalize(left) * deltaTime;
    }

    void MoveRight(float deltaTime)
    {
        pos -= glm::normalize(left) * deltaTime;
    }

    void MoveUp(float deltaTime)
    {
        pos += glm::normalize(worldUp) * deltaTime;
    }

    void MoveDown(float deltaTime)
    {
        pos -= glm::normalize(worldUp) * deltaTime;
    }

    void LookLeft(float detla)
    {
        dir = glm::normalize(detla * left + windowSize.width / 2 / glm::tan(horizontalFOV / 2) * dir);
        left = glm::normalize(glm::cross(worldUp, dir));
        up = glm::normalize(glm::cross(dir, left));
    }

    void LookRight(float detla)
    {
        dir = glm::normalize(detla * -left + windowSize.width / 2 / glm::tan(horizontalFOV / 2) * dir);
        left = glm::normalize(glm::cross(worldUp, dir));
        up = glm::normalize(glm::cross(dir, left));
    }

    void LookUp(float detla)
    {
        dir = glm::normalize(detla * up + windowSize.height / 2 / glm::tan(verticalFOV / 2) * dir);
        left = glm::normalize(glm::cross(worldUp, dir));
        up = glm::normalize(glm::cross(dir, left));
    }

    void LookDown(float detla)
    {
        dir = glm::normalize(detla * -up + windowSize.height / 2 / glm::tan(verticalFOV / 2) * dir);
        left = glm::normalize(glm::cross(worldUp, dir));
        up = glm::normalize(glm::cross(dir, left));
    }
};

Camera* mainCamera=new Camera();