#include "opxium/window.h"
#include <stdio.h>

static void framebuffer_resize_callback(GLFWwindow* handle, int width, int height) {
    OpxiumWindow* window = (OpxiumWindow*)glfwGetWindowUserPointer(handle);
    if (window) {
        window->framebufferResized = true;
        window->width = width;
        window->height = height;
    }
}

OpxiumResult opxium_window_create(OpxiumWindow* window, int width, int height, const char* title) {
    if (!glfwInit()) {
        fprintf(stderr, "[Opxium] Failed to initialize GLFW\n");
        return OPXIUM_ERROR_WINDOW_FAILED;
    }

    // Don't create OpenGL context - we're using Vulkan
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window->handle = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window->handle) {
        fprintf(stderr, "[Opxium] Failed to create GLFW window\n");
        glfwTerminate();
        return OPXIUM_ERROR_WINDOW_FAILED;
    }

    window->width = width;
    window->height = height;
    window->title = title;
    window->framebufferResized = false;

    // Set up user pointer for callbacks
    glfwSetWindowUserPointer(window->handle, window);
    glfwSetFramebufferSizeCallback(window->handle, framebuffer_resize_callback);

    printf("[Opxium] Window created: %dx%d\n", width, height);
    return OPXIUM_SUCCESS;
}

void opxium_window_destroy(OpxiumWindow* window) {
    if (window->handle) {
        glfwDestroyWindow(window->handle);
        window->handle = NULL;
    }
    glfwTerminate();
    printf("[Opxium] Window destroyed\n");
}

bool opxium_window_should_close(OpxiumWindow* window) {
    return glfwWindowShouldClose(window->handle);
}

void opxium_window_poll_events(void) {
    glfwPollEvents();
}

void opxium_window_get_framebuffer_size(OpxiumWindow* window, int* width, int* height) {
    glfwGetFramebufferSize(window->handle, width, height);
}

const char** opxium_window_get_required_extensions(uint32_t* count) {
    return glfwGetRequiredInstanceExtensions(count);
}
