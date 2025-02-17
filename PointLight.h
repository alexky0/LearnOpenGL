#pragma once

#include "Camera.h"
#include "Shader.h"

class PointLight {
private:
    glm::vec3 position, ambient, diffuse, specular;
    float constant, linear, quadratic;
public:
    PointLight(glm::vec3 pos, glm::vec3 col, float c, float l, float q);
    void UseLight(Shader& shader, const char* uniformName) const;
    void Render(Shader& shader, Camera& camera) const;
};
