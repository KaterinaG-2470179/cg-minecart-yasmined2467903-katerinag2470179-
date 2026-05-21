#define GLAD_GL_IMPLEMENTATION
#define GLFW_INCLUDE_NONE
#include <glad/gl.h>
#include <GLFW/glfw3.h>
 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
 
#include "Bezier.h"
#include "Camera.h"

Camera* g_camera = nullptr;

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
 
static std::string readFile(const char* path);
static unsigned int compileShader(GLenum type, const char* src);
static unsigned int createProgram(const char* vertPath, const char* fragPath);

float deltaTime = 0.0f;
float lastFrame = 0.0f;
 
int main()
{
    if (!glfwInit()) { return -1; }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
 
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Minecart", nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }
 
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
 
    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cerr << "Failed to init GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    unsigned int shaderProgram = createProgram("shaders/vertex.glsl", "shaders/fragment.glsl");

    Camera camera(SCR_WIDTH, SCR_HEIGHT);
    g_camera = &camera;
 
    Bezier bezier(
        glm::vec3(-3, 0, 0), glm::vec3(-1, 3, 0),
        glm::vec3( 1,-3, 0), glm::vec3( 3, 0, 0), 100
    );
    bezier.iterate();

    unsigned int vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
 
    glBindVertexArray(vao);
 
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 bezier.PointCount() * sizeof(glm::vec3),
                 bezier.GetPoints(),
                 GL_STATIC_DRAW);
 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)nullptr);
    glEnableVertexAttribArray(0);
 
    glBindVertexArray(0);
 
    int modelLoc      = glGetUniformLocation(shaderProgram, "model");
    int viewLoc       = glGetUniformLocation(shaderProgram, "view");
    int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
 
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime  = currentFrame - lastFrame;
        lastFrame  = currentFrame;

        camera.ProcessKeyboard(window, deltaTime);
 
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
        glUseProgram(shaderProgram);

        glm::mat4 model      = glm::mat4(1.0f);
        glm::mat4 view       = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(
            glm::radians(camera.GetFov()),
            (float)SCR_WIDTH / (float)SCR_HEIGHT,
            0.1f, 
            100.0f
        );
 
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
 
        glBindVertexArray(vao);
        glDrawArrays(GL_LINE_STRIP, 0, bezier.PointCount());
        glBindVertexArray(0);
 
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
}
 
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
 
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (g_camera)
        g_camera->ProcessMouseMovement(window, xpos, ypos);
}

static std::string readFile(const char* path)
{
    std::ifstream f(path);
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}
 
static unsigned int compileShader(GLenum type, const char* src)
{
    unsigned int id = glCreateShader(type);
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
 
    int ok;
    glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetShaderInfoLog(id, 512, nullptr, log);
        std::cerr << "Shader compile error:\n" << log << std::endl;
    }
    return id;
}
 
static unsigned int createProgram(const char* vertPath, const char* fragPath)
{
    std::string vertSrc = readFile(vertPath);
    std::string fragSrc = readFile(fragPath);

    // std::cerr << "vert: [" << vertSrc << "]" << std::endl;
    // std::cerr << "frag: [" << fragSrc << "]" << std::endl;
 
    unsigned int vert = compileShader(GL_VERTEX_SHADER,   vertSrc.c_str());
    unsigned int frag = compileShader(GL_FRAGMENT_SHADER, fragSrc.c_str());
 
    unsigned int program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);
 
    int ok;
    glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        std::cerr << "Program link error:\n" << log << std::endl;
    }
 
    glDeleteShader(vert);
    glDeleteShader(frag);
 
    return program;
}
