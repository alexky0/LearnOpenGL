#include "PointLight.h"

constexpr float lightVertices[] = {
    -0.1f, -0.1f, -0.1f,  0.1f, -0.1f, -0.1f,  0.1f,  0.1f, -0.1f,  -0.1f,  0.1f, -0.1f,
    -0.1f, -0.1f,  0.1f,  0.1f, -0.1f,  0.1f,  0.1f,  0.1f,  0.1f,   -0.1f,  0.1f,  0.1f
};

constexpr unsigned int lightIndices[] = {
    0, 2, 1, 0, 3, 2, 4, 5, 6, 4, 6, 7, 0, 1, 5, 5, 4, 0, 2, 3, 7, 7, 6, 2, 0, 4, 7, 7, 3, 0, 1, 2, 6, 6, 5, 1 
};

static unsigned int VAO = 0, VBO = 0, EBO = 0;

PointLight::PointLight(glm::vec3 pos, glm::vec3 col, float c, float l, float q)
    : position(pos), ambient(col * 0.2f), diffuse(col), specular(glm::mix(col, glm::vec3(1.0f), 0.5f)), constant(c), linear(l), quadratic(q)
{
    if (VAO == 0) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(lightVertices), lightVertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(lightIndices), lightIndices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }
}

void PointLight::UseLight(Shader& shader, const char* uniformName) const {
    string base = string(uniformName) + ".";
    shader.setVec3((base + "position").c_str(), position);
    shader.setVec3((base + "ambient").c_str(), ambient);
    shader.setVec3((base + "diffuse").c_str(), diffuse);
    shader.setVec3((base + "specular").c_str(), specular);
    shader.set1f((base + "constant").c_str(), constant);
    shader.set1f((base + "linear").c_str(), linear);
    shader.set1f((base + "quadratic").c_str(), quadratic);
}

void PointLight::Render(Shader& shader, Camera& camera) const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    shader.setVec3("color", diffuse);
    shader.setMat4fv("model", model);
    camera.Update(shader, "view", "projection");
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}
