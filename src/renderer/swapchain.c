#include "opxium/swapchain.h"
#include "opxium/device.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

SwapChainSupportDetails opxium_query_swapchain_support(VkPhysicalDevice device, VkSurfaceKHR surface) {
    SwapChainSupportDetails details = {0};
    
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    // Get formats
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &details.formatCount, NULL);
    if (details.formatCount > 0) {
        details.formats = malloc(details.formatCount * sizeof(VkSurfaceFormatKHR));
        if (details.formats) {
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &details.formatCount, details.formats);
        }
    }

    // Get present modes
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &details.presentModeCount, NULL);
    if (details.presentModeCount > 0) {
        details.presentModes = malloc(details.presentModeCount * sizeof(VkPresentModeKHR));
        if (details.presentModes) {
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &details.presentModeCount, details.presentModes);
        }
    }

    return details;
}

void opxium_free_swapchain_support_details(SwapChainSupportDetails* details) {
    if (details->formats) {
        free(details->formats);
        details->formats = NULL;
    }
    if (details->presentModes) {
        free(details->presentModes);
        details->presentModes = NULL;
    }
}

VkSurfaceFormatKHR opxium_choose_swap_surface_format(VkSurfaceFormatKHR* formats, uint32_t count) {
    // Prefer SRGB with 8-bit color
    for (uint32_t i = 0; i < count; i++) {
        if (formats[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
            formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return formats[i];
        }
    }
    // Fall back to first available
    return formats[0];
}

VkPresentModeKHR opxium_choose_swap_present_mode(VkPresentModeKHR* modes, uint32_t count) {
    // Prefer mailbox (triple buffering) for low latency
    for (uint32_t i = 0; i < count; i++) {
        if (modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            printf("[Opxium] Present mode: Mailbox (triple buffering)\n");
            return modes[i];
        }
    }
    // FIFO is guaranteed to be available
    printf("[Opxium] Present mode: FIFO (vsync)\n");
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D opxium_choose_swap_extent(const VkSurfaceCapabilitiesKHR* capabilities, OpxiumWindow* window) {
    if (capabilities->currentExtent.width != UINT32_MAX) {
        return capabilities->currentExtent;
    }

    int width, height;
    opxium_window_get_framebuffer_size(window, &width, &height);

    VkExtent2D actualExtent = {
        (uint32_t)width,
        (uint32_t)height
    };

    // Clamp to supported range
    if (actualExtent.width < capabilities->minImageExtent.width) {
        actualExtent.width = capabilities->minImageExtent.width;
    } else if (actualExtent.width > capabilities->maxImageExtent.width) {
        actualExtent.width = capabilities->maxImageExtent.width;
    }

    if (actualExtent.height < capabilities->minImageExtent.height) {
        actualExtent.height = capabilities->minImageExtent.height;
    } else if (actualExtent.height > capabilities->maxImageExtent.height) {
        actualExtent.height = capabilities->maxImageExtent.height;
    }

    return actualExtent;
}

OpxiumResult opxium_swapchain_create(
    VkPhysicalDevice physicalDevice,
    VkDevice device,
    VkSurfaceKHR surface,
    OpxiumWindow* window,
    OpxiumSwapchain* swapchain
) {
    SwapChainSupportDetails support = opxium_query_swapchain_support(physicalDevice, surface);

    VkSurfaceFormatKHR surfaceFormat = opxium_choose_swap_surface_format(support.formats, support.formatCount);
    VkPresentModeKHR presentMode = opxium_choose_swap_present_mode(support.presentModes, support.presentModeCount);
    VkExtent2D extent = opxium_choose_swap_extent(&support.capabilities, window);

    // Request one more image than minimum for triple buffering
    uint32_t imageCount = support.capabilities.minImageCount + 1;
    if (support.capabilities.maxImageCount > 0 && imageCount > support.capabilities.maxImageCount) {
        imageCount = support.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // Handle queue families
    QueueFamilyIndices indices = opxium_find_queue_families(physicalDevice, surface);
    uint32_t queueFamilyIndices[] = {(uint32_t)indices.graphicsFamily, (uint32_t)indices.presentFamily};

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = NULL;
    }

    createInfo.preTransform = support.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    opxium_free_swapchain_support_details(&support);

    VkResult result = vkCreateSwapchainKHR(device, &createInfo, NULL, &swapchain->handle);
    if (result != VK_SUCCESS) {
        fprintf(stderr, "[Opxium] Failed to create swapchain (error: %d)\n", result);
        return OPXIUM_ERROR_VULKAN_FAILED;
    }

    // Get swapchain images
    vkGetSwapchainImagesKHR(device, swapchain->handle, &swapchain->imageCount, NULL);
    swapchain->images = malloc(swapchain->imageCount * sizeof(VkImage));
    if (!swapchain->images) {
        return OPXIUM_ERROR_OUT_OF_MEMORY;
    }
    vkGetSwapchainImagesKHR(device, swapchain->handle, &swapchain->imageCount, swapchain->images);

    swapchain->imageFormat = surfaceFormat.format;
    swapchain->extent = extent;

    // Create image views
    swapchain->imageViews = malloc(swapchain->imageCount * sizeof(VkImageView));
    if (!swapchain->imageViews) {
        return OPXIUM_ERROR_OUT_OF_MEMORY;
    }

    for (uint32_t i = 0; i < swapchain->imageCount; i++) {
        VkImageViewCreateInfo viewInfo = {0};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = swapchain->images[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = swapchain->imageFormat;
        viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        result = vkCreateImageView(device, &viewInfo, NULL, &swapchain->imageViews[i]);
        if (result != VK_SUCCESS) {
            fprintf(stderr, "[Opxium] Failed to create image view (error: %d)\n", result);
            return OPXIUM_ERROR_VULKAN_FAILED;
        }
    }

    printf("[Opxium] Swapchain created: %dx%d, %d images\n", extent.width, extent.height, swapchain->imageCount);
    return OPXIUM_SUCCESS;
}

void opxium_swapchain_destroy(VkDevice device, OpxiumSwapchain* swapchain) {
    if (swapchain->imageViews) {
        for (uint32_t i = 0; i < swapchain->imageCount; i++) {
            vkDestroyImageView(device, swapchain->imageViews[i], NULL);
        }
        free(swapchain->imageViews);
        swapchain->imageViews = NULL;
    }

    if (swapchain->images) {
        free(swapchain->images);
        swapchain->images = NULL;
    }

    if (swapchain->handle != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(device, swapchain->handle, NULL);
        swapchain->handle = VK_NULL_HANDLE;
    }

    printf("[Opxium] Swapchain destroyed\n");
}
