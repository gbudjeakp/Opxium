#ifndef OPXIUM_TYPES_H
#define OPXIUM_TYPES_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct OpxiumEngine OpxiumEngine;
typedef struct OpxiumWindow OpxiumWindow;
typedef struct OpxiumRenderer OpxiumRenderer;

// Swap chain support details
typedef struct {
    VkSurfaceCapabilitiesKHR capabilities;
    uint32_t formatCount;
    VkSurfaceFormatKHR* formats;
    uint32_t presentModeCount;
    VkPresentModeKHR* presentModes;
} SwapChainSupportDetails;

// Queue family indices
typedef struct {
    int32_t graphicsFamily;
    int32_t presentFamily;
    bool graphicsFamilyHasValue;
    bool presentFamilyHasValue;
} QueueFamilyIndices;

// Engine configuration
typedef struct {
    const char* appName;
    uint32_t appVersion;
    int windowWidth;
    int windowHeight;
    bool enableValidation;
} OpxiumConfig;

// Result codes
typedef enum {
    OPXIUM_SUCCESS = 0,
    OPXIUM_ERROR_INIT_FAILED,
    OPXIUM_ERROR_WINDOW_FAILED,
    OPXIUM_ERROR_VULKAN_FAILED,
    OPXIUM_ERROR_DEVICE_NOT_FOUND,
    OPXIUM_ERROR_OUT_OF_MEMORY,
} OpxiumResult;

#endif // OPXIUM_TYPES_H
