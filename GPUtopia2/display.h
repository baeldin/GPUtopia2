#pragma once

#include "imgui.h"

// The application's per-frame entry point, called once per frame from main().
// The frame is assembled from the units in app_menu.h, layer_manager.h,
// layer_scheduler.h, layer_sync.h, compositor.h and viewport_input.h; the
// application state itself lives in app_state.h.
namespace mainView
{
	void mainViewPort(ImFont* font_mono);
};
