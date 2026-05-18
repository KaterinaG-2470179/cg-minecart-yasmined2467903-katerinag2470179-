#include <GLFW/glfw3.h>

int main() {

    // init glfw
    if (!glfwInit()) {
        return -1;
    }

    // create window
    GLFWwindow* window =
        glfwCreateWindow(1280, 720, "Minecart", nullptr, nullptr);

    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glViewport(0, 0, 1280, 720);
    glEnable(GL_DEPTH_TEST);

    // camera

    while(!glfwWindowShouldClose(window)) {
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render commandos

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // cleanup
    glfwTerminate();
    return 0;
}