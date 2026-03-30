#include "opxium/instance.h"
#include "opxium/window.h"
#include <stdio.h>
#include <string.h>

const char* const VALIDATION_LAYERS[] = {
    "VK_LAYER_KHRONOS_validation"
};
const uint32_t VALIDATION_LAYER_COUNT = 1;

bool opxium_check_validation_layer_support(void) {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, NULL);

    VkLayerProperties* availableLayers = malloc(layerCount * sizeof(VkLayerProperties));
    if (!availableLayers) {
        return false;
    }
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

    for (uint32_t i = 0; i < VALIDATION_LAYER_COUNT; i++) {
        bool layerFound = false;
        
        for (uint32_t j = 0; j < layerCount; j++) {
            if (strcmp(VALIDATION_LAYERS[i], availableLayers[j].layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            free(availableLayers);
            return false;
        }
    }

    free(availableLayers);
    return true;
}

OpxiumResult opxium_instance_create(VkInstance* instance, const OpxiumConfig* config) {
    // Check validation layer support if requested
    if (config->enableValidation && !opxium_check_validation_layer_support()) {
        fprintf(stderr, "[Opxium] Validation layers requested but not available\n");
        return OPXIUM_ERROR_VULKAN_FAILED;
    }

    // Application info
    VkApplicationInfo appInfo = {0};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = config->appName;
    appInfo.applicationVersion = config->appVersion;
    appInfo.pEngineName = "Opxium";
    appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;

    // Get required extensions from GLFW
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = opxium_window_get_required_extensions(&glfwExtensionCount);

    // Create instance info
    VkInstanceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

    // Enable validation layers if requested
    if (config->enableValidation) {
        createInfo.enabledLayerCount = VALIDATION_LAYER_COUNT;
        createInfo.ppEnabledLayerNames = VALIDATION_LAYERS;
        printf("[Opxium] Validation layers enabled\n");
    } else {
        createInfo.enabledLayerCount = 0;
    }

    // Create the Vulkan instance
    VkResult result = vkCreateInstance(&createInfo, NULL, instance);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "[Opxium] Failed to create Vulkan instance (error: %d)\n", result);
        return OPXIUM_ERROR_VULKAN_FAILED;
    }

    printf("[Opxium] Vulkan instance created\n");
    return OPXIUM_SUCCESS;
}

void opxium_instance_destroy(VkInstance instance) {
    if (instance != VK_NULL_HANDLE) {
        vkDestroyInstance(instance, NULL);
        printf("[Opxium] Vulkan instance destroyed\n");
    }
}
