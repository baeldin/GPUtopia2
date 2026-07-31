#include "layer_sync.h"

#include <iostream>

#include "log.h"

#include "history.h"
#include "layer_manager.h"

namespace layerSync
{
	void applyChanges(AppState& app)
	{
		FractalLayer& L = app.active();
		clFractal& cf = L.cf;
		clFractal& cf_old = L.cf_old;
		clLayerState& ls = L.kernelState;
		clCore& core = app.core;

		// only order rerun of imgKernel if flameRenderSettings change
		if (cf.frs != cf_old.frs and !cf.running())
		{
			logOut(LogLevel::Trace) << "Here?\n";
			cf.status.runImgKernel = true;
			if (L.historyIndex < L.history.size() - 1)
				popHistory(L.history, &L.historyIndex);
			L.history.push_back(json(cf.toExport()));
			L.historyIndex++;
			L.undone = false;
			L.redone = false;
			logOut(LogLevel::Trace) << "RENDER SETTINGS CHANGED: History index = " << L.historyIndex << " and length of history vector is " << L.history.size() << "\n";
			cf_old.frs = cf.frs;
		}
		//
		if (cf != cf_old) {
			cf.stop = true;
			// NOTE: the trailing semicolon makes this `if` a no-op, so the block
			// below always runs. Preserved verbatim; see the plan's bug list.
			if (!cf.running());
			{
				if (cf.newCLFragmentQueued())
				{
					if (cf.newFractalCLFragmentQueued())
						cf.popFractalCLFragmentQueue();
					if (cf.newOutsideColoringCLFragmentQueued())
						cf.popOutsideColoringCLFragmentQueue();
					if (cf.newInsideColoringCLFragmentQueued())
						cf.popInsideColoringCLFragmentQueue();
					if (cf.makeCLCode(NEW_FILES))
					{
						core.resetCore(ls);
						L.compilingFractalKernel = true;
						L.compileThread = std::jthread(&compileFractalKernelAsync, std::ref(core),
							std::ref(ls), std::ref(cf.fullCLcode), std::ref(L.compilingFractalKernel),
							std::ref(cf.status.runKernel));
						cf.buildKernel = false;
					}
					else
					{
						cf.status.filesOK = false;
						cf_old = cf;
						ImGui::OpenPopup("Could not open fragments");
					}
				}
				if (cf.useDouble != cf_old.useDouble ||
					(cf.mode >= 1) != (cf_old.mode >= 1))
				{
					cf.makeCLCode(SAME_FILES);
					core.resetCore(ls);
					L.compilingFractalKernel = true;
					L.compileThread = std::jthread(&compileFractalKernelAsync, std::ref(core),
						std::ref(ls), std::ref(cf.fullCLcode), std::ref(L.compilingFractalKernel),
						std::ref(cf.status.runKernel));
					cf.buildKernel = false;
				}
				core.setDefaultFractalArguments(ls, cf);
				core.setFractalParameterArgs(ls, cf);
				cf.image.updateComplexSubplane();
				cf.image.resetStatus();
				if (!L.compilingFractalKernel)
					cf.status.runKernel = true;
				cf.status.runImgKernel = false;
				cf.timings.erase(cf.timings.begin(), cf.timings.end());
				cf.status.done = false;
				logOut(LogLevel::Trace) << "Or here?\n";
				if (L.historyIndex < L.history.size() - 1 and !L.undone and !L.redone)
					popHistory(L.history, &L.historyIndex);
				L.history.push_back(json(cf.toExport()));
				L.historyIndex++;
				L.undone = false;
				L.redone = false;
				logOut(LogLevel::Trace) << "FRACTAL CHANGED: \nHistory index = " << L.historyIndex << " and length of history vector is " << L.history.size() << "\n";
				// Only propagate navigation to linked layers if navigation changed
				bool navChanged = (cf.image.center != cf_old.image.center ||
					cf.image.zoom != cf_old.image.zoom ||
					cf.image.angle != cf_old.image.angle);
				if (navChanged) {
					layers::propagateNavigation(app, L, layers::Center | layers::Zoom | layers::Angle);
				}
				cf_old = cf;
				cf.stop = false;
			}
		}
	}
}
