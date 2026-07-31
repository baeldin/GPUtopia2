#include "app_menu.h"

#include "clipboard.h"
#include "fileOperations.h"
#include "history.h"

namespace menu
{
	void drawMainMenuBar(AppState& app)
	{
		FractalLayer& L = app.active();
		clFractal& cf = L.cf;
		clFractal& cf_old = L.cf_old;
		clLayerState& ls = L.kernelState;
		clCore& core = app.core;

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New", "Ctrl+N"))
					newCLF(cf, core, ls);
				if (ImGui::MenuItem("Open", "Ctrl+O"))
					openCLF(cf, core, ls);
				if (ImGui::MenuItem("Save As", "Ctrl+Shift+S"))
					saveCLF(cf);
				if (ImGui::MenuItem("Export Image", "Ctrl+E"))
					savePNG(app.textureColors, cf.image.size);
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Undo", "Ctrl+Z"))
				{
					if (L.historyIndex > 0)
					{
						undo(cf, cf_old, core, ls, L.history, &L.historyIndex);
						L.redone = false;
						L.undone = true;
					}
				}
				if (ImGui::MenuItem("Redo", "Ctrl+Y"))
				{
					if (L.historyIndex < L.history.size() - 1)
					{
						redo(cf, cf_old, core, ls, L.history, &L.historyIndex);
						L.undone = false;
						L.redone = true;
					}
				}
				if (ImGui::MenuItem("TODO: Copy"))
				{
					json json = cf.toExport();
					std::string json_out = json.dump(4);
					CopyStringToClipboard(json.dump(4));
				}
				if (ImGui::MenuItem("TODO: Paste"))
				{
					std::string jsonStr = ReadStringFromClipboard();
					json json = json::parse(jsonStr);
					cf = clFractalContainer(json);
				}
				if (ImGui::MenuItem("TODO: Copy Image"))
				{
					// redo
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Fractal"))
			{
				if (ImGui::MenuItem("Reset Location"))
				{
					cf.image.center.x = 0.;
					cf.image.center.y = 0.;
					cf.image.zoom = 1.;
					cf.image.angle = 0.;
					cf.image.updateComplexSubplane();
				}
				ImGui::EndMenu();
			}
				ImGui::EndMainMenuBar();
		}
	}
}
