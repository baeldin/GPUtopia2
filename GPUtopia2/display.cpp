#include "display.h"

#include "app_menu.h"
#include "app_state.h"
#include "cl_error_window.h"
#include "compositor.h"
#include "flameRenderSettings.h"
#include "gradientWindow.h"
#include "imageSettings.h"
#include "info.h"
#include "layer_manager.h"
#include "layer_scheduler.h"
#include "layer_sync.h"
#include "settingsPanels.h"
#include "viewport_input.h"

namespace
{
	// Stop rendering and raise the error log when anything failed to build or parse.
	void drawErrorLog(AppState& app, ImFont* font_mono)
	{
		FractalLayer& L = app.active();
		clFractal& cf = L.cf;
		clLayerState& ls = L.kernelState;
		const bool hasError = (ls.imgKernel.errors.sum() != 0 || cf.parseError() != 0 || ls.fractalKernel.errors.sum() != 0);
		if (hasError)
		{
			ls.stop = true;
			cf.stop = true;
			app.showErrorWindow = true;
		}
		showErrorLogWindow(cf, ls, font_mono, hasError);
	}

	// The dockable tool windows, drawn in a fixed order.
	void drawPanels(AppState& app, ImFont* font_mono)
	{
		FractalLayer& L = app.active();
		clFractal& cf = L.cf;
		clLayerState& ls = L.kernelState;
		formulaSettingsWindow(cf, app.core, ls);
		outsideSettingsWindow(cf, app.core, ls);
		insideSettingsWindow(cf, app.core, ls);
		imageSettingsWindow(cf);
		flameRenderSettingsWindow(cf);
		infoWindow(cf, app.nav, font_mono);
		gradientWindow(cf);
		layers::drawLayerWindow(app);
	}
}

namespace mainView
{
	void mainViewPort(ImFont* font_mono)
	{
		static AppState app;

		layers::beginFrame(app);
		layers::syncViewportSize(app);
		layers::syncImageSize(app);
		scheduler::compileActiveIfNeeded(app);
		drawErrorLog(app, font_mono);
		menu::drawMainMenuBar(app);
		input::handleShortcuts(app);

		const clFractal& cf = app.active().cf;
		// The panels, navigation and compositing all run inside the main view
		// window: input::handleMouse() measures drags against its cursor origin.
		ImGui::Begin((cf.name + "###Main View").c_str(), nullptr, ImGuiWindowFlags_HorizontalScrollbar);
		drawPanels(app, font_mono);
		layerSync::applyChanges(app);
		glViewport(0, 0, app.mainViewportSize.x, app.mainViewportSize.y);
		scheduler::step(app);
		compositor::readbackCompositeAndUpload(app);
		input::handleMouse(app);
		ImGui::End();

		ImGui::Begin((cf.name + "###Main View").c_str(), nullptr, ImGuiWindowFlags_HorizontalScrollbar);
		ImGui::Image((void*)(intptr_t)app.textureID, ImVec2(
			cf.image.size.x, cf.image.size.y));
		ImGui::End();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
};
