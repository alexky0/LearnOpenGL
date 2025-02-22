#include "Object.h"
#include <iostream>
#include "Shader.h"

Object::Object(const char* path, Shader& shader, const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scl) : model(path), position(pos), rotation(rot), scale(scl), shader(&shader) {}

void Object::Update(Camera& camera, const char* viewUni, const char* projUni)
{
    camera.Update(*shader, viewUni, projUni);
    if (transformed)
    {
        matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, position);
        matrix = glm::rotate(matrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        matrix = glm::rotate(matrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        matrix = glm::rotate(matrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        matrix = glm::scale(matrix, scale);
        transformed = false;
    }
    shader->setMat4fv("model", matrix);
    model.Draw(*shader);
}

void Object::Update(Camera& camera, Shader shadowMap)
{
    if (transformed)
    {
        matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, position);
        matrix = glm::rotate(matrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        matrix = glm::rotate(matrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        matrix = glm::rotate(matrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        matrix = glm::scale(matrix, scale);
        transformed = false;
    }
    shadowMap.setMat4fv("model", matrix);
    model.Draw(shadowMap);
}

void Object::Move(float x, float y, float z)
{
	position += glm::vec3(x, y, z);
	transformed = true;
}

void Object::Rotate(float pitch, float yaw, float roll)
{
	rotation += glm::vec3(pitch, yaw, roll);
	transformed = true;
}

void Object::Scale(float x, float y, float z)
{
	scale *= glm::vec3(x, y, z);
	transformed = true;
}