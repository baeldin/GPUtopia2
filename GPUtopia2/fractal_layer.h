#pragma once

#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "cl_fractal.h"
#include "cl_core.h"
#include "color.h"

struct FractalLayer {
    clFractal cf;
    clFractal cf_old;
    clLayerState kernelState;
    std::vector<clFractalContainer> history;
    int historyIndex = -1;
    bool undone = false;
    bool redone = false;
    bool compilingFractalKernel = false;
    std::jthread compileThread;
    std::jthread runThread;
    std::vector<color> layerImage;
    // layerImage decoded to linear light, refreshed on every readback. The
    // compositor blends these: mixing the display-encoded values would make a
    // 50% blend land at ~21% of the light it should.
    std::vector<color> layerImageLinear;
    std::string name = "Layer 0";
    bool visible = true;
    float opacity = 1.0f;
    bool needsStartup = true;
    bool pendingRemoval = false;
    bool linkedNavigation = true; // pan/zoom/rotate together with active layer
};
