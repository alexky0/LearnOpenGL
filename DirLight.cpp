#include "DirLight.h"

DirLight::DirLight(glm::vec3 dir, glm::vec3 col) : direction(dir), ambient(col * 0.2f), diffuse(col), specular(glm::mix(col, glm::vec3(1.0f), 0.5f)) {}

void DirLight::UseLight(Shader& shader, const char* uniformName) const {
    string base = string(uniformName) + ".";
    shader.setVec3((base + "direction").c_str(), direction);
    shader.setVec3((base + "ambient").c_str(), ambient);
    shader.setVec3((base + "diffuse").c_str(), diffuse);
    shader.setVec3((base + "specular").c_str(), specular);
}