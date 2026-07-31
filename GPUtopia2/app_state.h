#pragma once

#include <memory>
#include <vector>

#include "GL/glew.h"
#include "imgui.h"

#include "cl_core.h"
#include "color.h"
#include "fractal_layer.h"
#include "fractal_navigation.h"

// Everything the main viewport needs to survive between frames. One instance
// lives as a function-local static in mainView::mainViewPort, so the OpenCL
// context is still created on the first frame rather than at static-init time.
// Not copyable: FractalLayer owns std::jthreads.
struct AppState
{
	// shared GPU infrastructure (single OpenCL context for all layers)
	clCore core;

	// layer system
	std::vector<std::unique_ptr<FractalLayer>> layers;
	int activeLayer = 0;
	// The layer active when the frame started. Selecting a different layer in
	// the Layers window changes activeLayer, but the rest of the frame keeps
	// working on the layer it began with; the switch takes effect next frame.
	FractalLayer* frameLayer = nullptr;

	// viewport texture and the images feeding it
	GLuint textureID = 0;
	bool updateTexture = false;
	std::vector<color> textureColors;     // result of compositing all visible layers
	std::vector<color> navPreviewImage;   // warped copy shown while dragging

	fractalNavigationParameters nav;
	ImVec2 mainViewportSize{};
	bool imgBlocked = false;
	bool showErrorWindow = true;

	// Valid from layers::beginFrame() onwards.
	FractalLayer& active() { return *frameLayer; }
};
