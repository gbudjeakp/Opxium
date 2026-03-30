#ifndef OPXIUM_WINDOW_H
#define OPXIUM_WINDOW_H

#include "types.h"

struct OpxiumWindow {
    GLFWwindow* handle;
    int width;
    int height;
    const char* title;
    bool framebufferResized;
};

OpxiumResult opxium_window_create(OpxiumWindow* window, int width, int height, const char* title);
void opxium_window_destroy(OpxiumWindow* window);
bool opxium_window_should_close(OpxiumWindow* window);
void opxium_window_poll_events(void);
void opxium_window_get_framebuffer_size(OpxiumWindow* window, int* width, int* height);
const char** opxium_window_get_required_extensions(uint32_t* count);

#endif // OPXIUM_WINDOW_H
