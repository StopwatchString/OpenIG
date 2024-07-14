#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "VkExtHelpers.h"

#include <iostream>
#include <vector>
#include <optional>
#include <set>
#include <unordered_map>

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

// Validation Layers we want to enable
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
// struct QueueFamilyIndices
//---------------------------------------------------------
struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

//---------------------------------------------------------
// isDeviceSuitable()
//---------------------------------------------------------
bool isDeviceSuitable(VkPhysicalDevice device, QueueFamilyIndices indices) {
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && indices.isComplete();
}

//---------------------------------------------------------
// getPhysicalDeviceCapabilitiesList()
//---------------------------------------------------------
std::unordered_map<VkPhysicalDevice, QueueFamilyIndices> getPhysicalDeviceCapabilitiesList(VkInstance& instance, VkSurfaceKHR& surface)
{
    std::unordered_map<VkPhysicalDevice, QueueFamilyIndices> deviceCapabilitiesList;

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        std::cout << "No GPUs with Vulkan support found!" << std::endl;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for (VkPhysicalDevice device : devices) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        for (uint32_t i = 0; i < queueFamilyCount; i++) {
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
            if (presentSupport) {
                indices.presentFamily = i;
            }

            if (indices.isComplete()) {
                break;
            }
        }


        deviceCapabilitiesList[device] = indices;
    }

    return deviceCapabilitiesList;
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

    // Surface implementation (required before physical devices)
    VkSurfaceKHR surface;

    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        std::cout << "Failed to create window surface using GLFW!" << std::endl;
        return EXIT_FAILURE;
    }

    // Physical device selection
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    std::unordered_map<VkPhysicalDevice, QueueFamilyIndices> physicalDeviceList = getPhysicalDeviceCapabilitiesList(instance, surface);

    for (const auto& [device, queueFamilyIndices] : physicalDeviceList) {
        if (isDeviceSuitable(device, queueFamilyIndices)) {
            physicalDevice = device;
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE) {
        std::cout << "Could not find suiteable device from list." << std::endl;
        return EXIT_FAILURE;
    }

    VkQueue presentQueue;

    QueueFamilyIndices queueIndices = physicalDeviceList[physicalDevice];

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = { queueIndices.graphicsFamily.value(), queueIndices.presentFamily.value() };
    float queuePriority = 1.0f;

    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.emplace_back(queueCreateInfo);
    }

    // Logical device selection
    VkDevice device;

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
    deviceCreateInfo.enabledExtensionCount = 0;

    if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS) {
        std::cout << "Issue creating logical device." << std::endl;
        return EXIT_FAILURE;
    }

    vkGetDeviceQueue(device, queueIndices.presentFamily.value(), 0, &presentQueue);

    //--------------
    // Swap Chain
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
    vkDestroyDevice(device, nullptr);
    
    vkDestroySurfaceKHR(instance, surface, nullptr);

    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }

    vkDestroyInstance(instance, nullptr);

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}