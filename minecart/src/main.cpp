#define GLAD_GL_IMPLEMENTATION
#define GLFW_INCLUDE_NONE

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "ShaderProgram.h"
#include "PostProcessor.h"
#include "Cube.h"
#include "Plane.h"
#include "Camera.h"
#include "Bezier.h"
#include "BezierMesh.h"

PostProcessMode mode = PostProcessMode::None;

Camera* g_camera = nullptr;
PostProcessor* g_post = nullptr;

const unsigned int SCR_WIDTH  = 1280;
const unsigned int SCR_HEIGHT = 720;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    if (g_post)
        g_post->resize(width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (g_camera)
        g_camera->ProcessMouseMovement(window, xpos, ypos);
}

void processPostProcessInput(GLFWwindow* window, PostProcessMode& mode)
{
    static bool pPressed = false;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && !pPressed)
    {
        if (mode == PostProcessMode::None)
            mode = PostProcessMode::Blur;
        else if (mode == PostProcessMode::Blur)
            mode = PostProcessMode::Sobel;
        else if (mode == PostProcessMode::Sobel)
            mode = PostProcessMode::ChromaKey;
        else
            mode = PostProcessMode::None;

        pPressed = true;
    }

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE)
        pPressed = false;
}

int main() {
    if (!glfwInit()) { return -1; }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Minecart", nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to init GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // objects in scene
    Camera camera(SCR_WIDTH, SCR_HEIGHT);
    g_camera = &camera;

    ShaderProgram sceneShader("shaders/vertex.vs", "shaders/fragment.fs");

    // cube 1
    const char* cubeTextures1[6] = {
        "resources/container.jpg", "resources/container.jpg",
        "resources/container.jpg", "resources/container.jpg",
        "resources/container.jpg", "resources/container.jpg"
    };
    Cube cube1(glm::vec3(-1.0f, 0.0f, -1.0f), 1.0f, cubeTextures1);

    // cube 2
    const char* cubeTextures2[6] = {
        "resources/container.jpg", "resources/container.jpg",
        "resources/container.jpg", "resources/container.jpg",
        "resources/container.jpg", "resources/container.jpg"
    };
    Cube cube2(glm::vec3(2.0f, 0.0f, 0.0f), 1.0f, cubeTextures2);

    // floor
    unsigned int floorTexture = loadTexture("resources/floor.jpg");
    Plane plane(floorTexture);

    // bezier
    Bezier bezier(
        glm::vec3(-3, 0, 0),
        glm::vec3(-1, 0, 3),
        glm::vec3( 1, 0,-3),
        glm::vec3( 3, 0, 0),
        100
    );
    bezier.iterate();

    BezierMesh bezierMesh(bezier);

    PostProcessor post(SCR_WIDTH, SCR_HEIGHT);
    g_post = &post;

    // render loop
    while (!glfwWindowShouldClose(window)) {
        
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        camera.ProcessKeyboard(window, deltaTime);

        processPostProcessInput(window, mode);
        post.setMode(mode);

        // pass 1: draw scene into FBO
        post.beginScene();

        sceneShader.use();
        sceneShader.setInt("tex", 0);

        int fbWidth, fbHeight;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

        glm::mat4 view       = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(
            glm::radians(camera.GetFov()),
            (float)fbWidth / (float)fbHeight,
            0.1f, 100.0f
        );
        sceneShader.setMat4("view",       view);
        sceneShader.setMat4("projection", projection);

        cube1.draw(sceneShader);
        cube2.draw(sceneShader);
        plane.draw(sceneShader);
        bezierMesh.draw(sceneShader);

        post.render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteTextures(1, &floorTexture);

    glfwTerminate();
    return 0;
}