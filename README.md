# Opxium Game Engine

A lightweight Vulkan-based game engine written in C.

## Features

- **Vulkan Rendering** - Modern GPU-accelerated rendering
- **GLFW Window Management** - Cross-platform window and input handling
- **Validation Layers** - Debug support with Vulkan validation
- **Modular Architecture** - Clean separation of concerns
- **Swapchain Management** - Proper present mode and format selection

## Project Structure

```
Opxium/
├── include/opxium/     # Public headers
│   ├── opxium.h        # Main include file
│   ├── types.h         # Common types and structures
│   ├── engine.h        # Engine lifecycle management
│   ├── window.h        # Window management
│   ├── instance.h      # Vulkan instance
│   ├── device.h        # Physical/logical device
│   ├── surface.h       # Window surface
│   └── swapchain.h     # Swapchain management
├── src/
│   ├── main.c          # Entry point
│   ├── core/           # Core engine systems
│   │   └── engine.c
│   ├── renderer/       # Vulkan rendering
│   │   ├── instance.c
│   │   ├── device.c
│   │   ├── surface.c
│   │   └── swapchain.c
│   └── platform/       # Platform abstraction
│       └── window.c
├── shaders/            # GLSL shader sources
├── build/              # Build output (generated)
└── Makefile
```

## Requirements

- GCC or Clang
- Vulkan SDK (1.2+)
- GLFW 3.3+
- pkg-config
- glslc (for shader compilation)

### Installing Dependencies

**Ubuntu/Debian:**
```bash
make deps
```

Or manually:
```bash
sudo apt-get install libglfw3-dev libvulkan-dev vulkan-validationlayers-dev glslc
```

**Arch Linux:**
```bash
sudo pacman -S glfw-x11 vulkan-devel glslang
```

**Fedora:**
```bash
sudo dnf install glfw-devel vulkan-devel glslc
```

## Building

```bash
# Debug build (default)
make

# Release build (optimized)
make release

# Build and run
make run

# Compile shaders
make shaders

# Clean build artifacts
make clean

# Full rebuild
make rebuild
```

## Usage

```c
#include "opxium/opxium.h"

int main() {
    OpxiumConfig config = opxium_default_config();
    config.appName = "My Game";
    config.windowWidth = 1280;
    config.windowHeight = 720;

    OpxiumEngine engine;
    if (opxium_engine_init(&engine, &config) != OPXIUM_SUCCESS) {
        return 1;
    }

    opxium_engine_run(&engine);
    opxium_engine_shutdown(&engine);

    return 0;
}
```

## Roadmap

- [ ] Graphics pipeline creation
- [ ] Command buffer recording
- [ ] Frame rendering
- [ ] Render pass management
- [ ] Framebuffer creation
- [ ] Depth buffering
- [ ] Texture loading
- [ ] Model loading
- [ ] Input handling
- [ ] ECS (Entity Component System)
- [ ] Audio system

## License

MIT License - See LICENSE file for details.
