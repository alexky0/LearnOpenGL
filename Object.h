#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Shader.h"
#include "Model.h"

class Object
{
private:
	bool transformed = true;
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
	glm::mat4 matrix = glm::mat4(1.0f);
	Model model;
	Shader* shader;
public:
	Object(const char* path, Shader& shader, const glm::vec3& pos = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec3& rot = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec3& scl = glm::vec3(1.0f, 1.0f, 1.0f));

	void Update(Camera& camera, const char* viewUni = "view", const char* projUni = "projection");
	void Update(Camera& camera, Shader& shadowMap);
	void Move(float x, float y, float z);
	void Rotate(float pitch, float yaw, float roll);
	void Scale(float x, float y, float z);
};