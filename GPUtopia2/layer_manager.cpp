#include "layer_manager.h"

#include <algorithm>
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
		// Deferred layer removal (safe: before binding references)
		app.layers.erase(std::remove_if(app.layers.begin(), app.layers.end(),
			[](const auto& l) { return l->pendingRemoval; }), app.layers.end());
		if (app.activeLayer >= (int)app.layers.size())
			app.activeLayer = (int)app.layers.size() - 1;
		app.frameLayer = app.layers[app.activeLayer].get();
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

	void drawLayerWindow(AppState& app)
	{
		auto& layers = app.layers;
		int& activeLayer = app.activeLayer;
		ImGui::Begin("Layers");
		if (ImGui::Button("Add Layer")) {
			auto nl = std::make_unique<FractalLayer>();
			nl->name = "Layer " + std::to_string(layers.size());
			// Copy navigation from active layer
			nl->cf.image = layers[activeLayer]->cf.image;
			nl->cf_old.image.zoom = -1.;  // force difference for startup
			layers.push_back(std::move(nl));
		}
		ImGui::SameLine();
		if (ImGui::Button("Remove Layer") && layers.size() > 1) {
			layers[activeLayer]->cf.stop = true;
			layers[activeLayer]->pendingRemoval = true;
		}
		ImGui::Separator();
		for (int i = 0; i < (int)layers.size(); i++) {
			ImGui::PushID(i);
			if (ImGui::Selectable(layers[i]->name.c_str(), i == activeLayer, ImGuiSelectableFlags_AllowItemOverlap, ImVec2(100, 0)))
				activeLayer = i;
			ImGui::SameLine();
			ImGui::Checkbox("##vis", &layers[i]->visible);
			ImGui::SameLine();
			ImGui::Checkbox("##nav", &layers[i]->linkedNavigation);
			if (ImGui::IsItemHovered()) ImGui::SetTooltip("Link navigation");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(100);
			ImGui::SliderFloat("##opacity", &layers[i]->opacity, 0.f, 1.f);
			ImGui::PopID();
		}
		ImGui::End();
	}
}
