#include "opxium/surface.h"
#include <stdio.h>

OpxiumResult opxium_surface_create(VkInstance instance, OpxiumWindow* window, VkSurfaceKHR* surface) {
    VkResult result = glfwCreateWindowSurface(instance, window->handle, NULL, surface);
    
    if (result != VK_SUCCESS) {
        fprintf(stderr, "[Opxium] Failed to create window surface (error: %d)\n", result);
        return OPXIUM_ERROR_VULKAN_FAILED;
    }

    printf("[Opxium] Window surface created\n");
    return OPXIUM_SUCCESS;
}

void opxium_surface_destroy(VkInstance instance, VkSurfaceKHR surface) {
    if (surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(instance, surface, NULL);
        printf("[Opxium] Window surface destroyed\n");
    }
}
