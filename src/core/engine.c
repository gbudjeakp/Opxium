#include "opxium/engine.h"
#include "opxium/instance.h"
#include "opxium/surface.h"
#include "opxium/device.h"
#include <stdio.h>
#include <string.h>

OpxiumConfig opxium_default_config(void) {
    OpxiumConfig config = {0};
    config.appName = "Opxium Application";
    config.appVersion = VK_MAKE_VERSION(1, 0, 0);
    config.windowWidth = 800;
    config.windowHeight = 600;
    config.enableValidation = true;
    return config;
}

OpxiumResult opxium_engine_init(OpxiumEngine* engine, const OpxiumConfig* config) {
    printf("[Opxium] Initializing engine...\n");

    // Clear engine state
    memset(engine, 0, sizeof(OpxiumEngine));
    engine->config = *config;

    OpxiumResult result;

    result = opxium_window_create(&engine->window, config->windowWidth, config->windowHeight, config->appName);
    if (result != OPXIUM_SUCCESS) {
        return result;
    }

    result = opxium_instance_create(&engine->instance, config);
    if (result != OPXIUM_SUCCESS) {
        opxium_window_destroy(&engine->window);
        return result;
    }

    result = opxium_surface_create(engine->instance, &engine->window, &engine->surface);
    if (result != OPXIUM_SUCCESS) {
        opxium_instance_destroy(engine->instance);
        opxium_window_destroy(&engine->window);
        return result;
    }

    result = opxium_pick_physical_device(engine->instance, engine->surface, &engine->physicalDevice);
    if (result != OPXIUM_SUCCESS) {
        opxium_surface_destroy(engine->instance, engine->surface);
        opxium_instance_destroy(engine->instance);
        opxium_window_destroy(&engine->window);
        return result;
    }

    result = opxium_create_logical_device(
        engine->physicalDevice,
        engine->surface,
        &engine->device,
        &engine->graphicsQueue,
        &engine->presentQueue,
        config->enableValidation
    );
    if (result != OPXIUM_SUCCESS) {
        opxium_surface_destroy(engine->instance, engine->surface);
        opxium_instance_destroy(engine->instance);
        opxium_window_destroy(&engine->window);
        return result;
    }

    result = opxium_swapchain_create(
        engine->physicalDevice,
        engine->device,
        engine->surface,
        &engine->window,
        &engine->swapchain
    );
    if (result != OPXIUM_SUCCESS) {
        opxium_destroy_logical_device(engine->device);
        opxium_surface_destroy(engine->instance, engine->surface);
        opxium_instance_destroy(engine->instance);
        opxium_window_destroy(&engine->window);
        return result;
    }

    engine->isInitialized = true;
    engine->isRunning = true;

    printf("[Opxium] Engine initialized successfully\n");
    return OPXIUM_SUCCESS;
}

void opxium_engine_run(OpxiumEngine* engine) {
    if (!engine->isInitialized) {
        fprintf(stderr, "[Opxium] Engine not initialized\n");
        return;
    }

    printf("[Opxium] Entering main loop\n");

    while (!opxium_window_should_close(&engine->window) && engine->isRunning) {
        opxium_window_poll_events();

        // TODO: Render frame
        // - Acquire swapchain image
        // - Record command buffer
        // - Submit to queue
        // - Present

        // Handle window resize
        if (engine->window.framebufferResized) {
            engine->window.framebufferResized = false;
            // TODO: Recreate swapchain
            printf("[Opxium] Window resized to %dx%d\n", engine->window.width, engine->window.height);
        }
    }

    // Wait for device to finish before cleanup
    vkDeviceWaitIdle(engine->device);

    printf("[Opxium] Exiting main loop\n");
}

void opxium_engine_shutdown(OpxiumEngine* engine) {
    if (!engine->isInitialized) {
        return;
    }

    printf("[Opxium] Shutting down engine...\n");

    // Destroy in reverse order of creation
    opxium_swapchain_destroy(engine->device, &engine->swapchain);
    opxium_destroy_logical_device(engine->device);
    opxium_surface_destroy(engine->instance, engine->surface);
    opxium_instance_destroy(engine->instance);
    opxium_window_destroy(&engine->window);

    engine->isInitialized = false;
    engine->isRunning = false;

    printf("[Opxium] Engine shutdown complete\n");
}
