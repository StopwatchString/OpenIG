#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <iostream>

const std::string WINDOW_NAME = "OpenIG";
const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

int main() {
    //--------------
    // Init GLFW
    //--------------
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Don't make an OpenGL context
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);   // TODO:: Implement window resizing

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, WINDOW_NAME.c_str(), nullptr, nullptr);

    //--------------
    // Init Vulkan
    //--------------

    //--------------
    // Main Loop
    //--------------
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    //--------------
    // Cleanup
    //--------------
    glfwDestroyWindow(window);
    glfwTerminate();


    return EXIT_SUCCESS;
}