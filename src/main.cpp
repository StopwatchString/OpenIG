#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "VkExtHelpers.h"
#include "DebugWindow.h"

#include <iostream>
#include <vector>
#include <optional>
#include <set>
#include <unordered_map>
#include <span>

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

// Validation Layers we want to enable
const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation",
};

// Required physicalDevice extensions
const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

const char* APPLICATION_NAME = "OpenIG";
constexpr uint32_t WIDTH = 800;
constexpr uint32_t HEIGHT = 600;

//---------------------------------------------------------
// getRequiredExtensions()
//---------------------------------------------------------
std::vector<const char*> getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    const std::span<const char*> extensionSpan(glfwExtensions, glfwExtensionCount);

    std::vector<const char*> extensions(extensionSpan.begin(), extensionSpan.end());

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

    bool const isComplete() noexcept {
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
// mapPhysicalDevicesToQueueFamilies()
//---------------------------------------------------------
std::unordered_map<VkPhysicalDevice, QueueFamilyIndices> mapPhysicalDevicesToQueueFamilies(VkInstance& instance, VkSurfaceKHR& surface)
{
    std::unordered_map<VkPhysicalDevice, QueueFamilyIndices> physicalDevicesToQueueFamilies;

    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
    if (physicalDeviceCount == 0) {
        std::cout << "No GPUs with Vulkan support found!" << std::endl;
    }

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

    for (VkPhysicalDevice physicalDevice : physicalDevices) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

        for (uint32_t i = 0; i < queueFamilyCount; i++) {
            // 
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
            if (presentSupport) {
                indices.presentFamily = i;
            }

            if (indices.isComplete()) {
                break;
            }
        }


        physicalDevicesToQueueFamilies[physicalDevice] = indices;
    }

    return physicalDevicesToQueueFamilies;
}


//---------------------------------------------------------
// main()
//---------------------------------------------------------
int main(int argc, char** argv) {

    DebugWindow debugWindow;
    debugWindow.init();
    float f1 = 0.0;
    debugWindow.addSliderFloat("test", f1, 0.0f, 10.0f);
    debugWindow.addSliderFloat("test", f1, 0.0f, 10.0f);
    debugWindow.addSliderFloat("test", f1, 0.0f, 10.0f);
    debugWindow.addSliderFloat("test", f1, 0.0f, 10.0f);
    debugWindow.addSliderFloat("test", f1, 0.0f, 10.0f);
    char string[] = "teststr";
    debugWindow.addInputText("some text", string, sizeof(string));

    debugWindow.addButton("print some text", []() {std::cout << "some text" << '\n'; });

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

    // Surface implementation (required before physical physicalDevices)
    VkSurfaceKHR surface;

    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        std::cout << "Failed to create window surface using GLFW!" << std::endl;
        return EXIT_FAILURE;
    }

    // Physical physicalDevice selection
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    std::unordered_map<VkPhysicalDevice, QueueFamilyIndices> physicalDeviceList = mapPhysicalDevicesToQueueFamilies(instance, surface);

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

    // deviceCreateInfo structs creations for logical physicalDevice selection
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

    // Logical physicalDevice selection
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

    VkQueue presentQueue;
    vkGetDeviceQueue(device, queueIndices.presentFamily.value(), 0, &presentQueue);

    //--------------
    // Swap Chain
    //--------------


    //--------------
    // Main Loop
    //--------------
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        debugWindow.draw();
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