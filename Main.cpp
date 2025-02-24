#include "Object.h"
#include "PointLight.h"
#include "DirLight.h"
#include "PostProcessing.h"
#include "Skybox.h"

using namespace std;

extern "C" {
    _declspec(dllexport) unsigned long NvOptimusEnablement = 1;
    _declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

constexpr const char* BACKPACK_PATH = "C:/Users/Alex/Downloads/Code/github/personal/LearnOpenGL/backpack/backpack.obj";
constexpr const char* WINDOW_PATH = "C:/Users/Alex/Downloads/Code/github/personal/LearnOpenGL/window/window.obj";
constexpr const char* FLOOR_PATH = "C:/Users/Alex/Downloads/Code/github/personal/LearnOpenGL/floor/floor.obj";

constexpr const unsigned int samples = 8;
constexpr const float gamma = 2.2f;

constexpr const unsigned int SCREEN_WIDTH = 1920, SCREEN_HEIGHT = 1080;
Camera camera(SCREEN_WIDTH, SCREEN_HEIGHT);
float currentTime, lastTime, deltaTime;
unsigned int frameCount = 0;

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
    glClearColor(pow(0.2f, gamma), pow(0.3f, gamma), pow(0.3f, gamma), pow(1.0f, gamma));
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

    PostProcessing post(SCREEN_WIDTH, SCREEN_HEIGHT, "postprocessing.vert", "postprocessing.frag", samples, gamma);

    DirLight dirLight(glm::vec3(0.0f, 0.5f, 0.5f), glm::vec3(0.2f, 0.2f, 0.2f));
    vector<PointLight> lights = {
        PointLight(glm::vec3(-1.299f, -0.75f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), 1.2f, 0.08f, 0.02f),
        PointLight(glm::vec3(+0.0f, -0.75f, 5.0f), glm::vec3(1.0f, 0.0f, 0.0f), 1.2f, 0.08f, 0.02f),
        PointLight(glm::vec3(+1.299f, -0.75f, 5.0f), glm::vec3(0.0f, 0.1f, 1.0f), 1.2f, 0.08f, 0.02f),
    };

    Shader shader("default.vert", "default.geom", "default.frag");
    Shader pointLight("light.vert", "light.frag");
    Shader dirShadows("shadowmap.vert", "shadowmap.frag");
    Shader pointShader("pointshadow.vert", "pointshadow.geom", "pointshadow.frag");

    Object backpack(BACKPACK_PATH, shader);
    Object myWindow(WINDOW_PATH, shader);
    myWindow.Rotate(90, 0, 0);
    myWindow.Move(1, 1, 1);
    Object floor(FLOOR_PATH, shader);
    floor.Move(0, -2, 0);
    std::vector<Object> objects = { backpack, floor, myWindow };
    Skybox skybox("skybox", "skybox.vert", "skybox.frag");

    while (!glfwWindowShouldClose(window))
    {
        currentTime = static_cast<float>(glfwGetTime());
        deltaTime = currentTime - lastTime;
        frameCount++;
        if (deltaTime >= 1.0 / 30.0)
        {
            string title = "LearnOpenGL - " + to_string((1.0 / deltaTime) * frameCount) + "FPS - " + to_string((deltaTime / frameCount) * 1000) + "ms";
            glfwSetWindowTitle(window, title.c_str());
            lastTime = currentTime;
            frameCount = 0;
        }
        camera.Keyboard(window, deltaTime);

        glEnable(GL_DEPTH_TEST);

        PointLight::ShadowPass(pointShader, camera, objects);

        dirLight.ShadowPass(dirShadows, camera);
        backpack.Update(camera, dirShadows);
        floor.Update(camera, dirShadows);
        myWindow.Update(camera, dirShadows);

        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        post.Setup();

        pointLight.Bind();
        for (PointLight& light : lights) light.Render(pointLight, camera);

        shader.Bind();
        dirLight.UseLight(shader, "dirLight");
        PointLight::UseLight(shader);
        shader.setVec3("viewPos", camera.getPosition());
        backpack.Update(camera);
        floor.Update(camera);
        glDisable(GL_CULL_FACE);
        myWindow.Update(camera);
        glEnable(GL_CULL_FACE);

        skybox.Update(camera, SCREEN_HEIGHT, SCREEN_WIDTH);

        post.Draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
	return 0;
}