#pragma once

#include "app_state.h"

namespace scheduler
{
	// Rebuild the active layer's fractal kernel if it was flagged, and show the
	// modal that blocks the UI while a kernel compiles.
	void compileActiveIfNeeded(AppState& app);

	// Advance every visible layer by at most one GPU operation per frame.
	// Work is serialised across layers: only one layer may compile, render or
	// tonemap at a time, because all layers share a single command queue.
	void step(AppState& app);
}
