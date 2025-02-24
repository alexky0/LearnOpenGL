#include "PointLight.h"

const glm::mat4 PointLight::shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near, far);
vector<glm::mat4> PointLight::shadowTransforms;
vector<PointLight> PointLight::lights;

PointLight::PointLight(glm::vec3 pos, glm::vec3 col, float c, float l, float q)
    : position(pos), ambient(col * 0.2f), diffuse(col), specular(glm::mix(col, glm::vec3(1.0f), 0.3f)), constant(c), linear(l), quadratic(q)
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

        glGenFramebuffers(1, &FBO);
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthMap);
        for (unsigned int i = 0; i < 6; ++i) glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    shadowTransforms.reserve(6);
    shadowTransforms.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(position, position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
    lights.push_back(*this);
}



void PointLight::ShadowPass(Shader& shader, Camera& camera, const vector<Object>& objs)
{
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    shader.Bind();
    for (size_t i = 0; i < lights.size(); i++)
    {
        shader.setVec3("lightPos", lights[i].getPosition());
        shader.set1f("far_plane", far);
        for (unsigned int j = 0; j < 6; j++) shader.setMat4fv(("shadowMatrices[" + std::to_string(j) + "]").c_str(), shadowTransforms[i * 6 + j]);
    }
    for (Object obj : objs) obj.Update(camera, shader);
}

void PointLight::UseLight(Shader& shader) {
    shader.set1i("numPointLights", (int)lights.size());
    glActiveTexture(GL_TEXTURE0 + 3);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthMap);
    shader.set1i("depthCubemap", 3);
    shader.set1f("far_plane", far);
    for (unsigned char i = 0; i < lights.size(); i++)
    {
        string base = string(("pointLights[" + to_string(i) + "]").c_str()) + ".";
        shader.setVec3((base + "position").c_str(), lights[i].position);
        shader.setVec3((base + "ambient").c_str(), lights[i].ambient);
        shader.setVec3((base + "diffuse").c_str(), lights[i].diffuse);
        shader.setVec3((base + "specular").c_str(), lights[i].specular);
        shader.set1f((base + "constant").c_str(), lights[i].constant);
        shader.set1f((base + "linear").c_str(), lights[i].linear);
        shader.set1f((base + "quadratic").c_str(), lights[i].quadratic);
    }
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

glm::vec3 PointLight::getPosition() const
{
    return position;
}
