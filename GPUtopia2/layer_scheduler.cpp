#include "layer_scheduler.h"

#include <iostream>

#include "log.h"

namespace scheduler
{
	void compileActiveIfNeeded(AppState& app)
	{
		FractalLayer& L = app.active();
		clFractal& cf = L.cf;
		clLayerState& ls = L.kernelState;
		// check if a kernel rebuild is needed (active layer)
		if (cf.buildKernel and ls.fractalKernel.errors.sum() == 0)
		{
			L.compilingFractalKernel = true;
			L.compileThread = std::jthread(&compileFractalKernelAsync, std::ref(app.core),
				std::ref(ls), std::ref(cf.fullCLcode), std::ref(L.compilingFractalKernel),
				std::ref(cf.status.runKernel));
			cf.buildKernel = false;
		}
		if (L.compilingFractalKernel)
		{
			ImGui::OpenPopup("Compiling");
		}
		// Begin the modal popup. This will block interaction with the rest of the UI.
		if (ImGui::BeginPopupModal("Compiling", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Compiling kernel... please wait.");
			ImGui::Text("This operation may take a few seconds.");
			if (!L.compilingFractalKernel)
			{
				logOut(LogLevel::Trace) << "Demanding kernel run\n";
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	void step(AppState& app)
	{
		FractalLayer& L = app.active();
		// Per-layer kernel execution loop (serialized: only one layer at a time)
		bool anyLayerBusy = false;
		for (auto& lp : app.layers) {
			if (lp->cf.running() || lp->compilingFractalKernel) {
				anyLayerBusy = true;
				break;
			}
		}
		for (auto& lp : app.layers) {
			FractalLayer& li = *lp;
			if (!li.visible) continue;
			// Inserted this frame by addLayer/duplicateLayer: no CL code and no
			// kernels yet, layers::beginFrame() builds them on the next frame.
			if (li.needsStartup) continue;
			// kernel rebuild for non-active layers
			if (&li != &L && li.cf.buildKernel && li.kernelState.fractalKernel.errors.sum() == 0 && !anyLayerBusy) {
				logErr(LogLevel::Trace) << "[LAYERS] " << li.name << ": compiling fractal kernel (non-active)\n";
				li.compilingFractalKernel = true;
				li.compileThread = std::jthread(&compileFractalKernelAsync, std::ref(app.core),
					std::ref(li.kernelState), std::ref(li.cf.fullCLcode), std::ref(li.compilingFractalKernel),
					std::ref(li.cf.status.runKernel));
				li.cf.buildKernel = false;
				anyLayerBusy = true;
			}
			if (li.cf.status.runKernel and !li.cf.running() and li.cf.image.current_sample_count < li.cf.image.target_sample_count and !anyLayerBusy) {
				logErr(LogLevel::Trace) << "[LAYERS] " << li.name << ": launching fractal kernel, samples=" << li.cf.image.current_sample_count << "\n";
				if (li.cf.image.current_sample_count == 0)
				{
					li.cf.image.resetStatus();
					logErr(LogLevel::Trace) << "[LAYERS] " << li.name << ": setDefaultFractalArguments\n";
					app.core.setDefaultFractalArguments(li.kernelState, li.cf);
					logErr(LogLevel::Trace) << "[LAYERS] " << li.name << ": setFractalParameterArgs\n";
					app.core.setFractalParameterArgs(li.kernelState, li.cf);
				}
				li.cf.status.runKernel = false;
				li.cf.status.kernelRunning = true;
				li.runThread = std::jthread(&runFractalKernelAsync, std::ref(li.cf), std::ref(app.core), std::ref(li.kernelState));
				anyLayerBusy = true;
			}
			if (li.cf.status.runImgKernel and !li.cf.running() and !anyLayerBusy and li.kernelState.currentRenderSize > 0) {
				logErr(LogLevel::Trace) << "[LAYERS] " << li.name << ": launching img kernel\n";
				li.cf.status.imgKernelRunning = true;
				li.runThread = std::jthread(&runImgKernelAsync, std::ref(li.cf), std::ref(app.core), std::ref(li.kernelState));
				li.cf.status.runImgKernel = false;
				anyLayerBusy = true;
			}
			// check current image quality (only if fractal kernel has run at least once)
			if (!li.cf.running() and !li.cf.status.done and li.kernelState.fractalKernel.errors.sum() == 0 and !li.compilingFractalKernel and li.cf.image.current_sample_count > 0)
			{
				if (li.cf.image.current_sample_count >= li.cf.image.next_update_sample_count)
				{
					while (li.cf.image.next_update_sample_count <= li.cf.image.current_sample_count)
						li.cf.image.next_update_sample_count *= 2;
					if (li.cf.image.next_update_sample_count > li.cf.image.target_sample_count)
						li.cf.image.next_update_sample_count = li.cf.image.target_sample_count;
					li.cf.status.runImgKernel = true;
				}
				else
				{
					li.cf.status.runKernel = true;
				}
			}
		}
	}
}
