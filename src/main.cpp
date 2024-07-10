#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <iostream>

const char* APPLICATION_NAME = "OpenIG";
const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

int main() {
    //--------------
    // Init GLFW
    //--------------
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Don't make an OpenGL context
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);   // TODO:: Implement window resizing

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, APPLICATION_NAME, nullptr, nullptr);

    //--------------
    // Init Vulkan
    //--------------

    // Create VKInstance
    VkInstance instance;

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = APPLICATION_NAME;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;


    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;
    createInfo.enabledLayerCount = 0;

    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

    if (result != VK_SUCCESS) {
        std::cout << "Failed to create VkInstance." << std::endl;
        return EXIT_FAILURE;
    }

    //--------------
    // Main Loop
    //--------------
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    //--------------
    // Cleanup
    //--------------

    vkDestroyInstance(instance, nullptr);

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}