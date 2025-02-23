#pragma once

#include "Shader.h"
#include "Camera.h"

constexpr const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
constexpr const float clampColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

class DirLight {
private:
    glm::vec3 direction, ambient, diffuse, specular;
    glm::mat4 lightProjection;
    unsigned int FBO, depthMap;
public:
    DirLight(glm::vec3 dir, glm::vec3 col);
    void ShadowPass(Shader& shader, Camera& camera) const;
    void UseLight(Shader& shader, const char* uniformName) const;
};
