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
    std::string name = "Layer 0";
    bool visible = true;
    float opacity = 1.0f;
    bool needsStartup = true;
    bool pendingRemoval = false;
    bool linkedNavigation = true; // pan/zoom/rotate together with active layer
};
