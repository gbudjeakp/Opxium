#include "opxium/device.h"
#include "opxium/instance.h"
#include "opxium/swapchain.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char* const DEVICE_EXTENSIONS[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};
const uint32_t DEVICE_EXTENSION_COUNT = 1;

QueueFamilyIndices opxium_find_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface) {
    QueueFamilyIndices indices = {0};
    indices.graphicsFamily = -1;
    indices.presentFamily = -1;
    indices.graphicsFamilyHasValue = false;
    indices.presentFamilyHasValue = false;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);

    VkQueueFamilyProperties* queueFamilies = malloc(queueFamilyCount * sizeof(VkQueueFamilyProperties));
    if (!queueFamilies) {
        return indices;
    }
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        // Check for graphics support
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
            indices.graphicsFamilyHasValue = true;
        }

        // Check for present support
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if (presentSupport) {
            indices.presentFamily = i;
            indices.presentFamilyHasValue = true;
        }

        // Found both, we're done
        if (indices.graphicsFamilyHasValue && indices.presentFamilyHasValue) {
            break;
        }
    }

    free(queueFamilies);
    return indices;
}

bool opxium_check_device_extension_support(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, NULL);

    VkExtensionProperties* availableExtensions = malloc(extensionCount * sizeof(VkExtensionProperties));
    if (!availableExtensions) {
        return false;
    }
    vkEnumerateDeviceExtensionProperties(device, NULL, &extensionCount, availableExtensions);

    // Check all required extensions are available
    for (uint32_t i = 0; i < DEVICE_EXTENSION_COUNT; i++) {
        bool found = false;
        for (uint32_t j = 0; j < extensionCount; j++) {
            if (strcmp(DEVICE_EXTENSIONS[i], availableExtensions[j].extensionName) == 0) {
                found = true;
                break;
            }
        }
        if (!found) {
            free(availableExtensions);
            return false;
        }
    }

    free(availableExtensions);
    return true;
}

bool opxium_is_device_suitable(VkPhysicalDevice device, VkSurfaceKHR surface) {
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    printf("[Opxium] Checking GPU: %s\n", deviceProperties.deviceName);

    // Check queue families
    QueueFamilyIndices indices = opxium_find_queue_families(device, surface);
    bool queuesComplete = indices.graphicsFamilyHasValue && indices.presentFamilyHasValue;

    // Check device extensions
    bool extensionsSupported = opxium_check_device_extension_support(device);

    // Check swapchain support
    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = opxium_query_swapchain_support(device, surface);
        swapChainAdequate = swapChainSupport.formatCount > 0 && swapChainSupport.presentModeCount > 0;
        opxium_free_swapchain_support_details(&swapChainSupport);
    }

    return queuesComplete && extensionsSupported && swapChainAdequate;
}

OpxiumResult opxium_pick_physical_device(VkInstance instance, VkSurfaceKHR surface, VkPhysicalDevice* physicalDevice) {
    *physicalDevice = VK_NULL_HANDLE;

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);

    if (deviceCount == 0) {
        fprintf(stderr, "[Opxium] No GPUs with Vulkan support found\n");
        return OPXIUM_ERROR_DEVICE_NOT_FOUND;
    }

    VkPhysicalDevice* devices = malloc(deviceCount * sizeof(VkPhysicalDevice));
    if (!devices) {
        return OPXIUM_ERROR_OUT_OF_MEMORY;
    }
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices);

    // First pass: look for discrete GPU
    for (uint32_t i = 0; i < deviceCount; i++) {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(devices[i], &props);
        
        if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && 
            opxium_is_device_suitable(devices[i], surface)) {
            *physicalDevice = devices[i];
            printf("[Opxium] Selected discrete GPU: %s\n", props.deviceName);
            break;
        }
    }

    // Second pass: accept integrated GPU if no discrete found
    if (*physicalDevice == VK_NULL_HANDLE) {
        for (uint32_t i = 0; i < deviceCount; i++) {
            if (opxium_is_device_suitable(devices[i], surface)) {
                VkPhysicalDeviceProperties props;
                vkGetPhysicalDeviceProperties(devices[i], &props);
                *physicalDevice = devices[i];
                printf("[Opxium] Selected GPU: %s\n", props.deviceName);
                break;
            }
        }
    }

    free(devices);

    if (*physicalDevice == VK_NULL_HANDLE) {
        fprintf(stderr, "[Opxium] No suitable GPU found\n");
        return OPXIUM_ERROR_DEVICE_NOT_FOUND;
    }

    return OPXIUM_SUCCESS;
}

OpxiumResult opxium_create_logical_device(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    VkDevice* device,
    VkQueue* graphicsQueue,
    VkQueue* presentQueue,
    bool enableValidation
) {
    QueueFamilyIndices indices = opxium_find_queue_families(physicalDevice, surface);

    // Determine unique queue families
    uint32_t uniqueQueueFamilies[2];
    uint32_t queueFamilyCount = 0;

    uniqueQueueFamilies[queueFamilyCount++] = indices.graphicsFamily;
    if (indices.graphicsFamily != indices.presentFamily) {
        uniqueQueueFamilies[queueFamilyCount++] = indices.presentFamily;
    }

    VkDeviceQueueCreateInfo* queueCreateInfos = malloc(queueFamilyCount * sizeof(VkDeviceQueueCreateInfo));
    if (!queueCreateInfos) {
        return OPXIUM_ERROR_OUT_OF_MEMORY;
    }

    float queuePriority = 1.0f;
    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        VkDeviceQueueCreateInfo queueCreateInfo = {0};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = uniqueQueueFamilies[i];
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos[i] = queueCreateInfo;
    }

    // Specify device features to use
    VkPhysicalDeviceFeatures deviceFeatures = {0};

    // Create the logical device
    VkDeviceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = queueFamilyCount;
    createInfo.pQueueCreateInfos = queueCreateInfos;
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = DEVICE_EXTENSION_COUNT;
    createInfo.ppEnabledExtensionNames = DEVICE_EXTENSIONS;

    // Validation layers (deprecated for devices but included for compatibility)
    if (enableValidation) {
        createInfo.enabledLayerCount = VALIDATION_LAYER_COUNT;
        createInfo.ppEnabledLayerNames = VALIDATION_LAYERS;
    } else {
        createInfo.enabledLayerCount = 0;
    }

    VkResult result = vkCreateDevice(physicalDevice, &createInfo, NULL, device);
    free(queueCreateInfos);

    if (result != VK_SUCCESS) {
        fprintf(stderr, "[Opxium] Failed to create logical device (error: %d)\n", result);
        return OPXIUM_ERROR_VULKAN_FAILED;
    }

    // Retrieve queue handles
    vkGetDeviceQueue(*device, indices.graphicsFamily, 0, graphicsQueue);
    vkGetDeviceQueue(*device, indices.presentFamily, 0, presentQueue);

    printf("[Opxium] Logical device created\n");
    return OPXIUM_SUCCESS;
}

void opxium_destroy_logical_device(VkDevice device) {
    if (device != VK_NULL_HANDLE) {
        vkDestroyDevice(device, NULL);
        printf("[Opxium] Logical device destroyed\n");
    }
}
