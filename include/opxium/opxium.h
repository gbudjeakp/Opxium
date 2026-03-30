#ifndef OPXIUM_H
#define OPXIUM_H

// Main include file for Opxium Engine
// Include this single header to access all engine functionality

#include "types.h"
#include "window.h"
#include "instance.h"
#include "device.h"
#include "surface.h"
#include "swapchain.h"
#include "engine.h"

// Version info
#define OPXIUM_VERSION_MAJOR 0
#define OPXIUM_VERSION_MINOR 1
#define OPXIUM_VERSION_PATCH 0

#define OPXIUM_MAKE_VERSION(major, minor, patch) \
    (((major) << 22) | ((minor) << 12) | (patch))

#define OPXIUM_VERSION OPXIUM_MAKE_VERSION(OPXIUM_VERSION_MAJOR, OPXIUM_VERSION_MINOR, OPXIUM_VERSION_PATCH)

#endif // OPXIUM_H
