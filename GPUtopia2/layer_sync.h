#pragma once

#include "app_state.h"

namespace layerSync
{
	// Compare the active layer against its previous state and react: re-run the
	// image kernel on render-setting changes, rebuild the kernel when a new CL
	// fragment or precision was selected, push an undo step, and propagate
	// navigation to linked layers.
	void applyChanges(AppState& app);
}
