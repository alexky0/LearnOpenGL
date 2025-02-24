#pragma once

#include "Camera.h"
#include "Shader.h"
#include "Object.h"

static unsigned int VAO = 0, VBO = 0, EBO = 0, FBO = 0, depthMap = 0;

class PointLight {
private:
    static constexpr const float lightVertices[] = { -0.1f, -0.1f, -0.1f,  0.1f, -0.1f, -0.1f,  0.1f,  0.1f, -0.1f,  -0.1f,  0.1f, -0.1f, -0.1f, -0.1f,  0.1f,  0.1f, -0.1f,  0.1f,  0.1f,  0.1f,  0.1f,   -0.1f,  0.1f,  0.1f };
    static constexpr const unsigned int lightIndices[] = { 0, 2, 1, 0, 3, 2, 4, 5, 6, 4, 6, 7, 0, 1, 5, 5, 4, 0, 2, 3, 7, 7, 6, 2, 0, 4, 7, 7, 3, 0, 1, 2, 6, 6, 5, 1 };
    
    static constexpr const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    static constexpr const float clampColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    static constexpr const float near = 1.0f;
    static constexpr const float far = 50.0f;
    static const glm::mat4 shadowProj;
    static vector<glm::mat4> shadowTransforms;
    static vector<PointLight> lights;
    
    glm::vec3 position, ambient, diffuse, specular;
    float constant, linear, quadratic;
public:
    PointLight(glm::vec3 pos, glm::vec3 col, float c, float l, float q);

    static void ShadowPass(Shader& shader, Camera& camera, const vector<Object>& objs);
    static void UseLight(Shader& shader);
    void Render(Shader& shader, Camera& camera) const;

    glm::vec3 getPosition() const;
};
