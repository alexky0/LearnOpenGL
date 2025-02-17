#include "Camera.h"

void Camera::UpdateCameraVectors()
{
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    front = glm::normalize(direction);
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}

Camera::Camera(int width, int height, glm::vec3 startPosition) :
    position(startPosition),
    worldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
    yaw(-90.0f),
    pitch(0.0f),
    front(glm::vec3(0.0f, 0.0f, -1.0f)),
    movementSpeed(2.5f),
    mouseSensitivity(0.1f),
    fov(90.0f),
    firstMouse(true),
    lastX(width / 2.0f),
    lastY(height / 2.0f),
    screenWidth(width),
    screenHeight(height)
{
    UpdateCameraVectors();
}

void Camera::Update(Shader shader, const char* viewUni, const char* projUni) const
{
    shader.setMat4fv(viewUni, glm::lookAt(position, position + front, up));
    shader.setMat4fv(projUni, glm::perspective(glm::radians(fov), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f));
}

void Camera::Keyboard(GLFWwindow* window, float deltaTime)
{
    float velocity = movementSpeed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)   glfwSetWindowShouldClose(window, GL_TRUE);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)   position += front * velocity;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)   position -= front * velocity;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)   position -= right * velocity;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)   position += right * velocity;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)   position += worldUp * velocity;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)   position -= worldUp * velocity;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)   movementSpeed = 1.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) movementSpeed = 2.5f;
}

void Camera::Mouse(float xpos, float ypos)
{
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    yaw += (xpos - lastX) * mouseSensitivity;
    pitch += (lastY - ypos) * mouseSensitivity;
    lastX = xpos;
    lastY = ypos;
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
    UpdateCameraVectors();
}

void Camera::Scroll(float yoffset)
{
    fov -= yoffset;
    if (fov < 1.0f) fov = 1.0f;
    if (fov > 90.0f) fov = 90.0f;
}

glm::vec3 Camera::getPosition() const
{
    return position;
}

glm::vec3 Camera::getFront() const
{
    return front;
}

glm::vec3 Camera::getUp() const
{
    return up;
}