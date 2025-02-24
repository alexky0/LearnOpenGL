#include "DirLight.h"

DirLight::DirLight(glm::vec3 dir, glm::vec3 col) : direction(dir), ambient(col * 0.8f), diffuse(col), specular(glm::mix(col, glm::vec3(1.0f), 0.5f))
{
    glGenFramebuffers(1, &FBO);
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glm::mat4 orthgonalProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 50.0f);
    glm::mat4 lightView = glm::lookAt(20.0f * glm::vec3(0.0f, 0.5f, 0.5f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    lightProjection = orthgonalProjection * lightView;
}

void DirLight::ShadowPass(Shader& shader, Camera& camera) const
{
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    shader.Bind();
    shader.setMat4fv("lightProjection", lightProjection);
}

void DirLight::UseLight(Shader& shader, const char* uniformName) const {
    shader.setMat4fv("lightProjection", lightProjection);
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    shader.set1i("shadowMap", 2);
    string base = string(uniformName) + ".";
    shader.setVec3((base + "direction").c_str(), direction);
    shader.setVec3((base + "ambient").c_str(), ambient);
    shader.setVec3((base + "diffuse").c_str(), diffuse);
    shader.setVec3((base + "specular").c_str(), specular);
}