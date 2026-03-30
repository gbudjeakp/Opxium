#ifndef OPXIUM_SURFACE_H
#define OPXIUM_SURFACE_H

#include "types.h"

// Surface functions
OpxiumResult opxium_surface_create(VkInstance instance, OpxiumWindow* window, VkSurfaceKHR* surface);
void opxium_surface_destroy(VkInstance instance, VkSurfaceKHR surface);

#endif // OPXIUM_SURFACE_H
