#pragma once

#include "app_state.h"

namespace input
{
	// Ctrl+N/O/Shift+S/E for file operations and Ctrl+Z / Ctrl+Y for undo/redo.
	void handleShortcuts(AppState& app);

	// Mouse navigation: Ctrl+drag pans, Shift+drag zooms, Alt+drag rotates.
	// While dragging, the last rendered image is warped as a preview; on release
	// the new location is committed and a re-render is queued.
	// Must be called while the main view window is current: the drag maths uses
	// ImGui::GetCursorScreenPos().
	void handleMouse(AppState& app);
}
