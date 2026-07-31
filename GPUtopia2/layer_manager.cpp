#include "layer_manager.h"

#include <algorithm>
#include <cfloat>
#include <cstdio>
#include <string>

#include "log.h"

namespace layers
{
	void beginFrame(AppState& app)
	{
		if (app.layers.empty()) {
			app.layers.push_back(std::make_unique<FractalLayer>());
			logErr(LogLevel::Trace) << "[LAYERS] Created default layer 0\n";
			auto nl = std::make_unique<FractalLayer>();
			nl->name = "Layer 1";
			nl->cf.image = app.layers[0]->cf.image;
			nl->cf_old.image.zoom = -1.;
			app.layers.push_back(std::move(nl));
			logErr(LogLevel::Trace) << "[LAYERS] Created default layer 1\n";
			const int pixelCount = app.layers[0]->cf.image.size.x * app.layers[0]->cf.image.size.y;
			app.textureColors.resize(pixelCount);
			app.navPreviewImage.resize(pixelCount);
		}
		// Deferred layer removal (safe: before binding references). Layers below
		// the selection shift down, so the selection has to shift with them or
		// it silently jumps to a different layer.
		int removedBelowActive = 0;
		for (int i = 0; i < app.activeLayer && i < (int)app.layers.size(); i++)
			if (app.layers[i]->pendingRemoval) removedBelowActive++;
		app.layers.erase(std::remove_if(app.layers.begin(), app.layers.end(),
			[](const auto& l) { return l->pendingRemoval; }), app.layers.end());
		app.activeLayer -= removedBelowActive;
		if (app.activeLayer >= (int)app.layers.size())
			app.activeLayer = (int)app.layers.size() - 1;
		if (app.activeLayer < 0)
			app.activeLayer = 0;
		FractalLayer* const previousFrameLayer = app.frameLayer;
		app.frameLayer = app.layers[app.activeLayer].get();
		app.layerChanged = (previousFrameLayer != app.frameLayer);
		if (app.layerChanged)
		{
			// The gradient window keeps its working copy in statics; this is the
			// flag it already watches for "the gradient underneath you was
			// replaced, drop your edits and resync".
			app.frameLayer->cf.gradient.isNew = true;
			logErr(LogLevel::Trace) << "[LAYERS] active layer is now " << app.frameLayer->name << "\n";
		}
		// Per-layer startup and imgKernel compilation
		for (size_t idx = 0; idx < app.layers.size(); idx++) {
			FractalLayer& li = *app.layers[idx];
			if (li.kernelState.imgKernel.need) {
				logErr(LogLevel::Trace) << "[LAYERS] " << li.name << ": compiling imgKernel\n";
				app.core.compileImgKernel(li.kernelState);
				logErr(LogLevel::Trace) << "[LAYERS] " << li.name << ": imgKernel compiled\n";
			}
			while (li.needsStartup)
			{
				logErr(LogLevel::Trace) << "[LAYERS] " << li.name << ": startup begin\n";
				if (li.cf.makeCLCode(NEW_FILES))
					li.cf.buildKernel = true;
				else
					li.kernelState.fractalKernel.errors.parseError = 1;
				logErr(LogLevel::Trace) << "[LAYERS] " << li.name << ": startup done, buildKernel=" << li.cf.buildKernel << "\n";
				li.needsStartup = false;
				// Force layerSync::applyChanges through its "fractal changed"
				// branch once, which is what seeds the undo history. A duplicate
				// arrives with its source's history already copied, so leaving
				// cf_old alone keeps it from re-running its linked layers.
				if (li.history.empty())
					li.cf_old.image.zoom = -1.; // just ANY difference will do for startup
			}
		}
	}

	void syncViewportSize(AppState& app)
	{
		if (app.mainViewportSize.x != ImGui::GetContentRegionAvail().x ||
			app.mainViewportSize.y != ImGui::GetContentRegionAvail().y)
		{
			app.mainViewportSize = ImGui::GetContentRegionAvail();
			app.mainViewportSize.y < 1 ? app.mainViewportSize.y = 1 : app.mainViewportSize.y;
		}
	}

	void syncImageSize(AppState& app)
	{
		clFractal& cf = app.active().cf;
		clFractal& cf_old = app.active().cf_old;
		if (cf_old.image.size != cf.image.size and !cf.running())
		{
			for (auto& lp : app.layers) {
				lp->cf.image.size = cf.image.size;
				lp->cf.image.updateComplexSubplane();
				lp->cf.image.resetStatus();
			}
			app.textureColors.resize(cf.image.size.x * cf.image.size.y);
			app.navPreviewImage.resize(cf.image.size.x * cf.image.size.y);
			cf_old.image = cf.image;
		}
	}

	void propagateNavigation(AppState& app, const FractalLayer& source, unsigned fields)
	{
		const clFractalImage& src = source.cf.image;
		for (auto& lp : app.layers) {
			FractalLayer& li = *lp;
			if (&li == &source || !li.linkedNavigation) continue;
			// A layer that is already there must be left alone: re-running it
			// would throw away every sample it has accumulated so far.
			const bool changes =
				((fields & Center) && li.cf.image.center != src.center) ||
				((fields & Zoom) && li.cf.image.zoom != src.zoom) ||
				((fields & Angle) && li.cf.image.angle != src.angle);
			if (!changes) continue;
			li.cf.stop = true;
			if (fields & Center) li.cf.image.center = src.center;
			if (fields & Zoom) li.cf.image.zoom = src.zoom;
			if (fields & Angle) li.cf.image.angle = src.angle;
			li.cf.image.updateComplexSubplane();
			li.cf.image.resetStatus();
			li.cf.status.runKernel = true;
			li.cf.status.runImgKernel = false;
			li.cf.status.done = false;
			li.cf.stop = false;
			li.cf_old = li.cf;
		}
	}

	std::string uniqueLayerName(const AppState& app, const std::string& base)
	{
		auto taken = [&app](const std::string& candidate) {
			for (const auto& lp : app.layers)
				if (lp->name == candidate) return true;
			return false;
		};
		if (!taken(base)) return base;
		for (int n = 2; ; n++) {
			std::string candidate = base + " " + std::to_string(n);
			if (!taken(candidate)) return candidate;
		}
	}

	int addLayer(AppState& app, int index)
	{
		auto nl = std::make_unique<FractalLayer>();
		nl->name = uniqueLayerName(app, "Layer " + std::to_string(app.layers.size()));
		// Copy navigation from the layer the new one is placed on
		nl->cf.image = app.layers[index]->cf.image;
		nl->cf_old.image.zoom = -1.;  // force difference for startup
		const int newIndex = index + 1;
		app.layers.insert(app.layers.begin() + newIndex, std::move(nl));
		if (app.activeLayer >= newIndex) app.activeLayer++;
		app.compositeDirty = true;
		return newIndex;
	}

	int duplicateLayer(AppState& app, int index)
	{
		const FractalLayer& src = *app.layers[index];
		auto nl = std::make_unique<FractalLayer>();
		// The fractal definition and everything the user tuned by hand
		nl->cf = src.cf;
		nl->history = src.history;
		nl->historyIndex = src.historyIndex;
		nl->layerImage = src.layerImage;   // shows the source image until it renders its own
		nl->layerImageLinear = src.layerImageLinear;
		nl->visible = src.visible;
		nl->opacity = src.opacity;
		nl->linkedNavigation = src.linkedNavigation;
		nl->name = uniqueLayerName(app, src.name + " copy");
		// Everything that refers to the source's GPU work must not come along.
		// kernelState stays default-constructed on purpose: own buffers, own
		// kernels, both flagged as needed.
		nl->cf.status = clFractalStatus{};
		nl->cf.stop = false;
		nl->cf.buildKernel = false;
		nl->cf.image.resetStatus();
		nl->cf.imgData.clear();
		nl->cf.imgIntRGBAData.clear();
		nl->cf.timings.clear();
		nl->cf_old = nl->cf;   // identical, so layerSync::applyChanges stays quiet
		nl->needsStartup = true;
		const int newIndex = index + 1;
		app.layers.insert(app.layers.begin() + newIndex, std::move(nl));
		app.activeLayer = newIndex;   // select the copy
		app.compositeDirty = true;
		logErr(LogLevel::Trace) << "[LAYERS] duplicated " << src.name << " as "
			<< app.layers[newIndex]->name << "\n";
		return newIndex;
	}

	void requestRemoveLayer(AppState& app, int index)
	{
		if (app.layers.size() < 2) return;
		app.layers[index]->cf.stop = true;
		app.layers[index]->pendingRemoval = true;
		app.compositeDirty = true;
	}

	void moveLayer(AppState& app, int index, int delta)
	{
		const int target = index + delta;
		if (target < 0 || target >= (int)app.layers.size()) return;
		// Only the unique_ptrs move, so frameLayer and any running threads are
		// untouched; just the composite order and the selection index change.
		std::iter_swap(app.layers.begin() + index, app.layers.begin() + target);
		if (app.activeLayer == index) app.activeLayer = target;
		else if (app.activeLayer == target) app.activeLayer = index;
		app.compositeDirty = true;
	}

	namespace
	{
		// A layer operation requested from a row while the row loop is still
		// walking app.layers. Applied once the loop has finished, because
		// insert/erase would invalidate the iteration.
		enum class LayerOp { None, Duplicate, Remove, MoveUp, MoveDown };

		void headerWithTooltip(int column, const char* label, const char* tooltip)
		{
			ImGui::TableSetColumnIndex(column);
			ImGui::TableHeader(label);
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", tooltip);
		}
	}

	void drawLayerWindow(AppState& app)
	{
		auto& layers = app.layers;
		int& activeLayer = app.activeLayer;
		// Which row is being renamed, and the edit buffer it writes into.
		static int renamingLayer = -1;
		static char renameBuffer[64] = {};
		static bool renameFocus = false;   // grab the keyboard on the first edit frame
		LayerOp op = LayerOp::None;
		int opIndex = -1;

		ImGui::Begin("Layers");

		// --- toolbar -------------------------------------------------------
		if (ImGui::Button("Add"))
			activeLayer = addLayer(app, activeLayer);
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Add an empty layer above the selected one");
		ImGui::SameLine();
		if (ImGui::Button("Duplicate")) { op = LayerOp::Duplicate; opIndex = activeLayer; }
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Copy the selected layer, settings and all");
		ImGui::SameLine();
		ImGui::BeginDisabled(layers.size() < 2);
		if (ImGui::Button("Remove")) { op = LayerOp::Remove; opIndex = activeLayer; }
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Delete the selected layer");
		ImGui::EndDisabled();
		ImGui::SameLine();
		ImGui::BeginDisabled(activeLayer >= (int)layers.size() - 1);
		if (ImGui::ArrowButton("##up", ImGuiDir_Up)) { op = LayerOp::MoveUp; opIndex = activeLayer; }
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Move the selected layer towards the front");
		ImGui::EndDisabled();
		ImGui::SameLine();
		ImGui::BeginDisabled(activeLayer <= 0);
		if (ImGui::ArrowButton("##down", ImGuiDir_Down)) { op = LayerOp::MoveDown; opIndex = activeLayer; }
		if (ImGui::IsItemHovered()) ImGui::SetTooltip("Move the selected layer towards the back");
		ImGui::EndDisabled();

		// --- layer table ---------------------------------------------------
		const ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg |
			ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit;
		if (ImGui::BeginTable("##layers", 4, tableFlags))
		{
			ImGui::TableSetupColumn("Layer", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Show", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Link", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Opacity", ImGuiTableColumnFlags_WidthFixed, 120.f);
			// Drawn by hand rather than with TableHeadersRow() so every header
			// can explain what its column does.
			ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
			headerWithTooltip(0, "Layer", "Click to select, double-click to rename.\nRight-click for more.");
			headerWithTooltip(1, "Show", "Visible. Hidden layers are left out of the\nimage and stop rendering.");
			headerWithTooltip(2, "Link", "Linked navigation: pan, zoom and rotate\nfollow the selected layer.");
			headerWithTooltip(3, "Opacity", "How strongly the layer covers the ones behind it.");

			// Front-most layer first: the stack is stored back-to-front.
			for (int i = (int)layers.size() - 1; i >= 0; i--)
			{
				FractalLayer& li = *layers[i];
				ImGui::PushID(i);
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				if (renamingLayer == i)
				{
					if (renameFocus)
					{
						ImGui::SetKeyboardFocusHere();
						renameFocus = false;
					}
					ImGui::SetNextItemWidth(-FLT_MIN);
					const bool committed = ImGui::InputText("##rename", renameBuffer, sizeof(renameBuffer),
						ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);
					if (committed || ImGui::IsItemDeactivated())
					{
						std::string wanted(renameBuffer);
						if (!wanted.empty() && wanted != li.name)
							li.name = uniqueLayerName(app, wanted);
						renamingLayer = -1;
					}
				}
				else
				{
					// The selectable spans the whole row, so double-click-to-rename
					// has to be restricted to the name cell, or double-clicking the
					// opacity slider would start a rename too.
					const float nameCellRightEdge = ImGui::GetCursorScreenPos().x +
						ImGui::GetContentRegionAvail().x;
					const ImGuiSelectableFlags flags = ImGuiSelectableFlags_SpanAllColumns |
						ImGuiSelectableFlags_AllowItemOverlap;
					if (ImGui::Selectable(li.name.c_str(), i == activeLayer, flags))
						activeLayer = i;
					// imgui 1.88: the flag alone is not enough, the row also has
					// to yield hover priority to the widgets drawn on top of it.
					ImGui::SetItemAllowOverlap();
					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) &&
						ImGui::GetIO().MousePos.x < nameCellRightEdge)
					{
						renamingLayer = i;
						renameFocus = true;
						std::snprintf(renameBuffer, sizeof(renameBuffer), "%s", li.name.c_str());
					}
					if (ImGui::BeginPopupContextItem("##layermenu"))
					{
						activeLayer = i;
						if (ImGui::MenuItem("Duplicate")) { op = LayerOp::Duplicate; opIndex = i; }
						if (ImGui::MenuItem("Rename"))
						{
							renamingLayer = i;
							renameFocus = true;
							std::snprintf(renameBuffer, sizeof(renameBuffer), "%s", li.name.c_str());
						}
						if (ImGui::MenuItem("Move up", nullptr, false, i < (int)layers.size() - 1))
						{
							op = LayerOp::MoveUp; opIndex = i;
						}
						if (ImGui::MenuItem("Move down", nullptr, false, i > 0))
						{
							op = LayerOp::MoveDown; opIndex = i;
						}
						ImGui::Separator();
						if (ImGui::MenuItem("Delete", nullptr, false, layers.size() > 1))
						{
							op = LayerOp::Remove; opIndex = i;
						}
						ImGui::EndPopup();
					}
				}

				ImGui::TableSetColumnIndex(1);
				if (ImGui::Checkbox("##vis", &li.visible))
					app.compositeDirty = true;
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("Visible. Hidden layers are left out of the\nimage and stop rendering.");

				ImGui::TableSetColumnIndex(2);
				ImGui::Checkbox("##nav", &li.linkedNavigation);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("Linked navigation: pan, zoom and rotate\nfollow the selected layer.");

				ImGui::TableSetColumnIndex(3);
				ImGui::SetNextItemWidth(-FLT_MIN);
				if (ImGui::SliderFloat("##opacity", &li.opacity, 0.f, 1.f, "%.2f"))
					app.compositeDirty = true;

				ImGui::PopID();
			}
			ImGui::EndTable();
		}
		ImGui::End();

		// Applied after the loop: insert and erase would invalidate it.
		switch (op)
		{
		case LayerOp::Duplicate: duplicateLayer(app, opIndex); renamingLayer = -1; break;
		case LayerOp::Remove:    requestRemoveLayer(app, opIndex); renamingLayer = -1; break;
		case LayerOp::MoveUp:    moveLayer(app, opIndex, +1); break;
		case LayerOp::MoveDown:  moveLayer(app, opIndex, -1); break;
		case LayerOp::None:      break;
		}
	}
}
