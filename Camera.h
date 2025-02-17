#pragma once
#include <glm/ext/vector_float3.hpp>
#include "Shader.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

class Camera
{
private:
	int screenWidth, screenHeight;
	glm::vec3 position, front, up, right, worldUp, direction;
	float yaw, pitch, lastX, lastY, movementSpeed, mouseSensitivity, fov;
	bool firstMouse;
	void UpdateCameraVectors();
public:
	Camera(int width, int height, glm::vec3 startPosition = glm::vec3(0.0f, 0.0f, 3.0f));

	void Update(Shader shader, const char* viewUni, const char* projUni) const;
	void Keyboard(GLFWwindow* window, float deltaTime);
	void Mouse(float xpos, float ypos);
	void Scroll(float yoffset);

	glm::vec3 getPosition() const;
	glm::vec3 getFront() const;
	glm::vec3 getUp() const;
};