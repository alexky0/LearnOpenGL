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
    Shader lightShader("light.vert", "light.frag");

    Object backpack(BACKPACK_PATH, shader);
    Object myWindow(WINDOW_PATH, shader);
    myWindow.Rotate(90, 0, 0);
    myWindow.Move(1, 1, 1);
    Object floor(FLOOR_PATH, shader);
    floor.Move(0, -2, 0);

    Skybox skybox("skybox", "skybox.vert", "skybox.frag");

    unsigned int shadowMapFBO;
    glGenFramebuffers(1, &shadowMapFBO);
    constexpr const unsigned int shadowMapWidth = 1024, shadowMapHeight = 1024;
    unsigned int shadowMap;
    glGenTextures(1, &shadowMap);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float clampColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glm::mat4 orthgonalProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 50.0f);
    glm::mat4 lightView = glm::lookAt(20.0f * glm::vec3(0.0f, 0.5f, 0.5f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightProjection = orthgonalProjection * lightView;
    Shader shadowMapProgram("shadowmap.vert", "shadowmap.frag");

    unsigned int pointLightDepthMapFBO;
    glGenFramebuffers(1, &pointLightDepthMapFBO);
    unsigned int depthCubemap;
    glGenTextures(1, &depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    for (unsigned int i = 0; i < 6; ++i) glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);
    glBindFramebuffer(GL_FRAMEBUFFER, pointLightDepthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    float aspect = (float)shadowMapWidth / (float)shadowMapHeight;
    float near = 1.0f;
    float far = 50.0f;
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far);
    std::vector<glm::mat4> shadowTransforms;
    shadowTransforms.reserve(lights.size() * 6);
    for (const PointLight& light : lights) {
        glm::vec3 lightPos = light.getPosition();
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
    }
    Shader pointShader("pointshadow.vert", "pointshadow.geom", "pointshadow.frag");

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
        glViewport(0, 0, shadowMapWidth, shadowMapHeight);
        pointShader.Bind();
        glBindFramebuffer(GL_FRAMEBUFFER, pointLightDepthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        for (size_t i = 0; i < lights.size(); i++)
        {
            pointShader.setVec3("lightPos", lights[i].getPosition());
            pointShader.set1f("far_plane", far);
            for (unsigned int j = 0; j < 6; j++)
            {
                string uniformName = "shadowMatrices[" + std::to_string(j) + "]";
                pointShader.setMat4fv(uniformName.c_str(), shadowTransforms[i * 6 + j]);
            }
            backpack.Update(camera, pointShader);
            floor.Update(camera, pointShader);
            myWindow.Update(camera, pointShader);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        shadowMapProgram.Bind();
        shadowMapProgram.setMat4fv("lightProjection", lightProjection);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        backpack.Update(camera, shadowMapProgram);
        floor.Update(camera, shadowMapProgram);
        myWindow.Update(camera, shadowMapProgram);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        post.Setup();

        lightShader.Bind();
        for (PointLight& light : lights) light.Render(lightShader, camera);

        shader.Bind();
        shader.setMat4fv("lightProjection", lightProjection);
        glActiveTexture(GL_TEXTURE0 + 2);
        glBindTexture(GL_TEXTURE_2D, shadowMap);
        shader.set1i("shadowMap", 2);
        glActiveTexture(GL_TEXTURE0 + 3);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
        shader.set1i("depthCubemap", 3);
        shader.set1f("far_plane", far);
        dirLight.UseLight(shader, "dirLight");
        for (unsigned char i = 0; i < lights.size(); i++) lights[i].UseLight(shader, ("pointLights[" + to_string(i) + "]").c_str());
        shader.set1i("numPointLights", (int)lights.size());
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