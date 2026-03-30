#ifndef OPXIUM_SWAPCHAIN_H
#define OPXIUM_SWAPCHAIN_H

#include "types.h"

// Swapchain structure
typedef struct {
    VkSwapchainKHR handle;
    VkFormat imageFormat;
    VkExtent2D extent;
    uint32_t imageCount;
    VkImage* images;
    VkImageView* imageViews;
} OpxiumSwapchain;

// Swapchain support query
SwapChainSupportDetails opxium_query_swapchain_support(VkPhysicalDevice device, VkSurfaceKHR surface);
void opxium_free_swapchain_support_details(SwapChainSupportDetails* details);

// Swapchain functions
OpxiumResult opxium_swapchain_create(
    VkPhysicalDevice physicalDevice,
    VkDevice device,
    VkSurfaceKHR surface,
    OpxiumWindow* window,
    OpxiumSwapchain* swapchain
);
void opxium_swapchain_destroy(VkDevice device, OpxiumSwapchain* swapchain);

// Swapchain configuration helpers
VkSurfaceFormatKHR opxium_choose_swap_surface_format(VkSurfaceFormatKHR* formats, uint32_t count);
VkPresentModeKHR opxium_choose_swap_present_mode(VkPresentModeKHR* modes, uint32_t count);
VkExtent2D opxium_choose_swap_extent(const VkSurfaceCapabilitiesKHR* capabilities, OpxiumWindow* window);

#endif // OPXIUM_SWAPCHAIN_H
