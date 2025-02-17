#include <iostream>

#include "Object.h"
#include "PointLight.h"
#include "DirLight.h"

using namespace std;

extern "C" {
    _declspec(dllexport) unsigned long NvOptimusEnablement = 1;
    _declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

constexpr const char* BACKPACK_PATH = "C:/Users/Alex/Downloads/Code/github/personal/LearnOpenGL/backpack/backpack.obj";

constexpr unsigned int SCREEN_WIDTH = 1920, SCREEN_HEIGHT = 1080;
Camera camera(SCREEN_WIDTH, SCREEN_HEIGHT);
float currentTime, lastTime, deltaTime;

static void FramebufferSizeCallback(GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); }
static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos) { camera.Mouse((float)xpos, (float)ypos); }
static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) { camera.Scroll((float)yoffset); }
static GLFWwindow* WindowInit()
{
    if (!glfwInit())
    {
        cout << "Failed to initialize GLFW" << endl;
        return nullptr;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL", glfwGetPrimaryMonitor(), NULL);
    if (!window)
    {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return nullptr;
    }
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetCursorPosCallback(window, CursorPosCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    stbi_set_flip_vertically_on_load(true);
    return window;
}

int main()
{
    GLFWwindow* window = WindowInit();
    if (!window) return -1;

    Shader shader("default.vert", "default.frag");
    Shader lightShader("light.vert", "light.frag");

    Object backpack(BACKPACK_PATH, shader);

    DirLight dirLight(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.98f, 0.84f, 0.11f));
    vector<PointLight> lights = {
        PointLight(glm::vec3(+0.0f, +1.5f, +0.5f), glm::vec3(0.0f, 0.0f, 1.0f), 1.5f, 0.07f, 0.017f),
        PointLight(glm::vec3(-1.299f, -0.75f, +0.5f), glm::vec3(1.0f, 0.0f, 0.0f), 1.2f, 0.08f, 0.02f),
        PointLight(glm::vec3(+1.299f, -0.75f, +0.5f), glm::vec3(0.0f, 1.0f, 0.0f), 1.3f, 0.09f, 0.025f),
    };

    while (!glfwWindowShouldClose(window))
    {
        currentTime = static_cast<float>(glfwGetTime());
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        camera.Keyboard(window, deltaTime);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightShader.Bind();
        for (PointLight& light : lights) light.Render(lightShader, camera);

        shader.Bind();
        for (unsigned char i = 0; i < lights.size(); i++) lights[i].UseLight(shader, ("pointLights[" + to_string(i) + "]").c_str());
        shader.set1i("numPointLights", (int)lights.size());
        dirLight.UseLight(shader, "dirLight");
        shader.setVec3("viewPos", camera.getPosition());
        backpack.Update(camera);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
	return 0;
}