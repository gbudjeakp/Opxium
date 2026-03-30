#ifndef OPXIUM_ENGINE_H
#define OPXIUM_ENGINE_H

#include "types.h"
#include "window.h"
#include "swapchain.h"

// Main engine structure
struct OpxiumEngine {
    OpxiumConfig config;
    OpxiumWindow window;
    
    // Vulkan core
    VkInstance instance;
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    
    // Queues
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    
    // Swapchain
    OpxiumSwapchain swapchain;
    
    // State
    bool isRunning;
    bool isInitialized;
};

// Engine lifecycle
OpxiumResult opxium_engine_init(OpxiumEngine* engine, const OpxiumConfig* config);
void opxium_engine_run(OpxiumEngine* engine);
void opxium_engine_shutdown(OpxiumEngine* engine);

// Default configuration
OpxiumConfig opxium_default_config(void);

#endif 
