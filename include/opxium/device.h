#ifndef OPXIUM_DEVICE_H
#define OPXIUM_DEVICE_H

#include "types.h"

extern const char* const DEVICE_EXTENSIONS[];
extern const uint32_t DEVICE_EXTENSION_COUNT;

OpxiumResult opxium_pick_physical_device(VkInstance instance, VkSurfaceKHR surface, VkPhysicalDevice* physicalDevice);
bool opxium_is_device_suitable(VkPhysicalDevice device, VkSurfaceKHR surface);
QueueFamilyIndices opxium_find_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface);
bool opxium_check_device_extension_support(VkPhysicalDevice device);

OpxiumResult opxium_create_logical_device(
    VkPhysicalDevice physicalDevice,
    VkSurfaceKHR surface,
    VkDevice* device,
    VkQueue* graphicsQueue,
    VkQueue* presentQueue,
    bool enableValidation
);
void opxium_destroy_logical_device(VkDevice device);

#endif 
