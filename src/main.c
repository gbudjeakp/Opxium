/*
 * Opxium Game Engine
 * Main Entry Point
 *
 * A Vulkan-based game engine written in C.
 */

#include "opxium/opxium.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    printf("Opxium Engine v%d.%d.%d\n", 
           OPXIUM_VERSION_MAJOR, 
           OPXIUM_VERSION_MINOR, 
           OPXIUM_VERSION_PATCH);
    printf("=====================================\n\n");

    // Configure the engine
    OpxiumConfig config = opxium_default_config();
    config.appName = "Opxium Demo";
    config.windowWidth = 1280;
    config.windowHeight = 720;
    config.enableValidation = true;

    // Initialize the engine
    OpxiumEngine engine;
    OpxiumResult result = opxium_engine_init(&engine, &config);
    
    if (result != OPXIUM_SUCCESS) {
        fprintf(stderr, "Failed to initialize engine (error: %d)\n", result);
        return EXIT_FAILURE;
    }

    // Run the main loop
    opxium_engine_run(&engine);

    // Cleanup
    opxium_engine_shutdown(&engine);

    return EXIT_SUCCESS;
}
