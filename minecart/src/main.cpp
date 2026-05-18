#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#include "Bezier.h"

int main() {

    // init glfw
    if (!glfwInit()) { return -1; }

    // create window
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Minecart", nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }

    glfwMakeContextCurrent(window);
    glViewport(0, 0, 1280, 720);
    glEnable(GL_DEPTH_TEST);

    // camera

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-5, 5, -5, 5, -100, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    Bezier bezier(
        glm::vec3(-3, 0, 0),
        glm::vec3(-1, 3, 0),
        glm::vec3( 1, -3, 0),
        glm::vec3( 3, 0, 0),
        100
    );
    bezier.iterate();

    while(!glfwWindowShouldClose(window)) {
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render commandos

        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i < bezier.PointCount(); i++) {
            glm::vec3 p = bezier.GetPoint(i);
            glVertex3f(p.x, p.y, p.z);
        }
        glEnd();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // cleanup
    glfwTerminate();
    return 0;
}