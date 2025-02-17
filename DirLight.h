#pragma once

#include "Shader.h"

class DirLight {
private:
    glm::vec3 direction, ambient, diffuse, specular;
public:
    DirLight(glm::vec3 dir, glm::vec3 col);
    void UseLight(Shader& shader, const char* uniformName) const;
};
