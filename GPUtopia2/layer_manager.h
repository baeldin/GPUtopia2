#pragma once

#include "app_state.h"

namespace layers
{
	// Which navigation values a layer copies from the layer being navigated.
	enum NavField : unsigned
	{
		Center = 1u,
		Zoom = 2u,
		Angle = 4u
	};

	// Create the default layers on the first frame, drop layers flagged for
	// removal, clamp the active index and run per-layer startup (imgKernel
	// compilation and the initial CL code generation).
	void beginFrame(AppState& app);

	// Track the size of the dockspace the fractal is drawn into.
	void syncViewportSize(AppState& app);

	// If the active layer's image size changed, apply it to every layer and
	// resize the CPU-side image buffers.
	void syncImageSize(AppState& app);

	// Copy the requested navigation values from `source` to every other layer
	// with linked navigation, and queue those layers for a re-render.
	void propagateNavigation(AppState& app, const FractalLayer& source, unsigned fields);

	// The "Layers" window: add/remove, select, visibility, link, opacity.
	void drawLayerWindow(AppState& app);
}
