#define GLAD_GL_IMPLEMENTATION
#define GLFW_INCLUDE_NONE

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <memory>

#include "TextureUtil.h"
#include "ShaderProgram.h"
#include "PostProcessor.h"
#include "Torch.h"
#include "Plane.h"
#include "Camera.h"
#include "Bezier.h"
#include "BezierMesh.h"
#include "PointLight.h"

PostProcessMode mode = PostProcessMode::None;

Camera* g_camera = nullptr;
PostProcessor* g_post = nullptr;

const unsigned int SCR_WIDTH  = 1280;
const unsigned int SCR_HEIGHT = 720;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool nightMode = false;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    if (g_post)
        g_post->resize(width, height);
}

static const glm::vec3 s_torchPositions[] = {
    glm::vec3(-4.0f, 0.0f, -2.0f),
    glm::vec3( 3.0f, 0.0f,  0.0f),
    glm::vec3( 1.0f, 0.0f,  4.0f),
    glm::vec3(-2.0f, 0.0f,  1.0f),
};
static const int NUM_TORCHES = sizeof(s_torchPositions) / sizeof(s_torchPositions[0]);
bool torchActive[NUM_TORCHES] = { true, true, true, true };

bool rayHitsSphere(glm::vec3 rayOrigin, glm::vec3 rayDir, glm::vec3 center, float radius) {
    glm::vec3 oc = rayOrigin - center;
    float b = glm::dot(oc, rayDir);
    float c = glm::dot(oc, oc) - radius * radius;
    return (b * b - c) >= 0.0f;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (g_camera)
        g_camera->ProcessMouseMovement(window, xpos, ypos);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button != GLFW_MOUSE_BUTTON_LEFT || action != GLFW_PRESS) return;

    double mx, my;
    glfwGetCursorPos(window, &mx, &my);

    int fbW, fbH;
    glfwGetFramebufferSize(window, &fbW, &fbH);

    float ndcX = (2.0f * (float)mx) / fbW - 1.0f;
    float ndcY = 1.0f - (2.0f * (float)my) / fbH;

    glm::mat4 proj = glm::perspective(
        glm::radians(g_camera->GetFov()),
        (float)fbW / (float)fbH,
        0.1f, 100.0f
    );
    glm::mat4 view = g_camera->GetViewMatrix();

    glm::vec4 clipRay = glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
    glm::vec4 eyeRay = glm::inverse(proj) * clipRay;
    eyeRay = glm::vec4(eyeRay.x, eyeRay.y, -1.0f, 0.0f);
    glm::vec3 worldRay = glm::normalize(glm::vec3(glm::inverse(view) * eyeRay));
    glm::vec3 rayOrigin = g_camera->GetPosition();

    for (int i = 0; i < NUM_TORCHES; i++) {
        glm::vec3 torchCenter = s_torchPositions[i] + glm::vec3(0.0f, 3.0f, 0.0f);
        float dist = glm::length(rayOrigin - torchCenter);
        if (rayHitsSphere(rayOrigin, worldRay, torchCenter, 0.6f)) {
            torchActive[i] = !torchActive[i];
            break;
        }
    }
}

void processPostProcessInput(GLFWwindow* window, PostProcessMode& mode) {
    static bool pPressed = false;
    static bool bPressed = false;
    static bool nPressed = false;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && !pPressed)     {
        if (mode == PostProcessMode::None)
            mode = PostProcessMode::Blur;
        else if (mode == PostProcessMode::Blur)
            mode = PostProcessMode::Sobel;
        else if (mode == PostProcessMode::Sobel)
            mode = PostProcessMode::ChromaKey;
        else if (mode == PostProcessMode::ChromaKey)
            mode = PostProcessMode::Bloom;
        else
            mode = PostProcessMode::None;

        pPressed = true;
    }

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !bPressed) {
        if (mode == PostProcessMode::Bloom) 
            mode = PostProcessMode::None;
        else
            mode = PostProcessMode::Bloom;

        bPressed = true;
    }

    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS && !nPressed) {
        nightMode = !nightMode;
        nPressed = true;
    }

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE) pPressed = false;
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE) bPressed = false;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_RELEASE) nPressed = false;
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
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to init GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Camera camera(SCR_WIDTH, SCR_HEIGHT);
    g_camera = &camera;

    ShaderProgram sceneShader("shaders/vertex.vs", "shaders/fragment.fs");

    // Floor
    Plane plane;

    // Torches
    std::unique_ptr<Torch> torches[NUM_TORCHES];
    std::vector<PointLight> lights;

    for (int i = 0; i < NUM_TORCHES; i++) {
        torches[i] = std::make_unique<Torch>(s_torchPositions[i]);
        lights.emplace_back(s_torchPositions[i] + glm::vec3(0.0f, 2.85f, 0.0f));
    }

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
        post.setNightMode(nightMode);

        post.beginScene();

        sceneShader.use();
        sceneShader.setInt("tex", 0);

        int fbWidth, fbHeight;
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(
            glm::radians(camera.GetFov()),
            (float)fbWidth / (float)fbHeight,
            0.1f, 100.0f
        );
        sceneShader.setMat4("view", view);
        sceneShader.setMat4("projection", projection);

        if (nightMode) {
            // dim blueish moon light
            sceneShader.setVec3("dirLight.direction", 0.4f, 1.0f, 0.3f);
            sceneShader.setVec3("dirLight.ambient", 0.02f, 0.02f, 0.05f);
            sceneShader.setVec3("dirLight.diffuse", 0.05f, 0.05f, 0.15f);
            sceneShader.setVec3("dirLight.specular", 0.05f, 0.05f, 0.1f);
        } else {
            // normal sun
            sceneShader.setVec3("dirLight.direction", 0.4f, 1.0f, 0.3f);
            sceneShader.setVec3("dirLight.ambient", 0.35f, 0.35f, 0.30f);
            sceneShader.setVec3("dirLight.diffuse", 1.0f, 0.98f, 0.90f);
            sceneShader.setVec3("dirLight.specular", 0.6f, 0.6f, 0.55f);
        }

        // torch lights
        float torchIntensity = nightMode ? 3.0f : 1.0f;
        int activeLights = 0;

        for (int i = 0; i < NUM_TORCHES; i++) {
            if (torchActive[i])
                lights[i].upload(sceneShader, activeLights++, torchIntensity);
        }

        sceneShader.setInt("numPointLights", activeLights);

        for (int i = 0; i < NUM_TORCHES; i++)
            torches[i]->draw(sceneShader);
            
        plane.draw(sceneShader);
        bezierMesh.draw(sceneShader);

        post.render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}