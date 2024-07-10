#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "VkExtHelpers.h"

#include <iostream>
#include <vector>

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation",
};

const char* APPLICATION_NAME = "OpenIG";
const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

//---------------------------------------------------------
// getRequiredExtensions()
//---------------------------------------------------------
std::vector<const char*> getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

//---------------------------------------------------------
// main()
//---------------------------------------------------------
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

    // Enable validation layers if needed
    uint32_t layerCount = 0;

    if (enableValidationLayers) {
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        bool foundAllValidationLayers = true;
        std::vector<const char*> missingLayers;

        for (const char* layerName : validationLayers) {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                missingLayers.push_back(layerName);
            }
        }

        if (!missingLayers.empty()) {
            std::cout << "Missing requested validation layers:" << std::endl;
            for (const char* layerName : missingLayers) {
                std::cout << layerName << std::endl;
            }
            return EXIT_FAILURE;
        }
    }

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
    std::vector<const char*> extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    createInfo.enabledLayerCount = enableValidationLayers ? static_cast<uint32_t>(validationLayers.size()) : 0;
    createInfo.ppEnabledLayerNames = enableValidationLayers ? validationLayers.data() : nullptr;
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfoInstance{};
    populateDebugMessengerCreateInfo(debugCreateInfoInstance);
    createInfo.pNext = enableValidationLayers ? &debugCreateInfoInstance : nullptr;

    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

    if (result != VK_SUCCESS) {
        std::cout << "Failed to create VkInstance." << std::endl;
        return EXIT_FAILURE;
    }

    VkDebugUtilsMessengerEXT debugMessenger;
    if (enableValidationLayers) {
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        populateDebugMessengerCreateInfo(debugCreateInfo);

        if (CreateDebugUtilsMessengerEXT(instance, &debugCreateInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
            std::cout << "Failed to set up debug messenger." << std::endl;
            return EXIT_FAILURE;
        }
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
    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }

    vkDestroyInstance(instance, nullptr);

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}