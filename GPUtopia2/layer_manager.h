#pragma once

#include <string>

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

	// `base`, or `base 2`, `base 3`, ... - whichever no layer is called yet.
	std::string uniqueLayerName(const AppState& app, const std::string& base);

	// Insert a fresh default layer directly above `index`, inheriting only the
	// navigation of the layer it is placed on. Returns the new layer's index.
	int addLayer(AppState& app, int index);

	// Insert a copy of layer `index` directly above it. The copy gets its own
	// kernels and buffers; only the fractal definition is shared by value.
	// Returns the new layer's index.
	int duplicateLayer(AppState& app, int index);

	// Flag layer `index` for removal in the next layers::beginFrame(). Does
	// nothing if it is the only layer left.
	void requestRemoveLayer(AppState& app, int index);

	// Move layer `index` one step through the stack: delta +1 towards the front,
	// -1 towards the back. Does nothing at the ends.
	void moveLayer(AppState& app, int index, int delta);

	// The "Layers" window: add/duplicate/remove/reorder/rename, select,
	// visibility, navigation link, opacity.
	void drawLayerWindow(AppState& app);
}
