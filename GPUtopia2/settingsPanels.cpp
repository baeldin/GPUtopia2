#include "settingsPanels.h"

#include <filesystem>
#include <string>
#include <vector>

#include "imgui.h"

#include "cl_core.h"
#include "cl_fractal.h"
#include "selectFile.h"

namespace
{
	// Window header shared by all three panels: title, "load fragment" dialog,
	// the name of the fragment currently in use, and the reload button.
	// The caller continues drawing into the window and closes it with End().
	void beginFragmentPanel(const char* windowTitle, const char* loadButtonLabel,
		const std::string& activeFile, std::string& uiFile,
		clFractal& cf, clCore& cc, clLayerState& ls)
	{
		ImGui::Begin(windowTitle);
		std::string fragmentName = std::filesystem::path(activeFile).stem().string();
		if (ImGui::Button(loadButtonLabel))
		{
			bool success = false;
			std::string newFragment;
			openFileDialog(newFragment, success, "cl", "GPUtopia CL Fragment (*.cl)");
			if (success)
			{
				uiFile = newFragment;
			}
		}
		ImGui::Text(fragmentName.c_str());
		if (ImGui::Button("Reload CL Fragments"))
		{
			cf.makeCLCode(SAME_FILES);
			cf.buildKernel = true;
			cc.resetCore(ls);
		}
	}

	// One widget per parameter, grouped by type: int, real, complex, enum.
	void drawParameterWidgets(parameterMaps& params)
	{
		for (auto& [key, val] : params.integerParameters)
		{
			ImGui::DragInt(key.c_str(), &(val.first), 1, 1, 10000);
		}
		for (auto& [key, val] : params.realParameters)
		{
			ImGui::InputDouble(key.c_str(), &(val.first));
		}
		for (auto& [key, val] : params.complexParameters)
		{
			std::string labelR = key + ".R";
			std::string labelI = key + ".I";
			ImGui::InputDouble(labelR.c_str(), &(val.first.x));
			ImGui::InputDouble(labelI.c_str(), &(val.first.y));
		}
		for (auto& [key, val] : params.enumParameters)
		{
			std::vector<const char*> enumLabels;
			for (const auto& label : val.first.labels) {
				enumLabels.push_back(label.c_str());
			}
			ImGui::Combo(key.c_str(), &val.first.value, enumLabels.data(), static_cast<int>(enumLabels.size()));
		}
	}
}

void formulaSettingsWindow(clFractal& cf, clCore& cc, clLayerState& ls)
{
	beginFragmentPanel("Formulas", "Load Formula",
		cf.fractalCLFragmentFile, cf.fractalCLFragmentFileUi, cf, cc, ls);
	ImGui::SameLine();
	if (ImGui::Button("Load AA diag"))
	{
		cf.fractalCLFragmentFileUi = "clFragments/diag_AA.cl";
		cf.insideColoringCLFragmentFileUi = "clFragments/diag_AA_col.cl";
		cf.outsideColoringCLFragmentFileUi = "clFragments/diag_AA_col.cl";
	}
	// Seeded from cf every frame on purpose: a static would carry the previous
	// layer's setting over on a layer switch and force a kernel rebuild with it.
	bool useDouble = cf.useDouble;
	if (ImGui::Checkbox("Double Precision", &useDouble))
	{
		cf.useDouble = useDouble;
		cf.makeCLCode(SAME_FILES);
		cf.buildKernel = true;
		cc.resetCore(ls);
	}
	ImGui::DragFloat("Bailout", &cf.bailout, 4., 20000.);
	ImGui::Text("Formula Parameters:");
	drawParameterWidgets(cf.params.fractalParameterMaps);
	ImGui::Checkbox("Vomit pixel contents", &cf.vomit);
	ImGui::End();
}

void insideSettingsWindow(clFractal& cf, clCore& cc, clLayerState& ls)
{
	beginFragmentPanel("Inside", "Load Coloring Algorithm",
		cf.insideColoringCLFragmentFile, cf.insideColoringCLFragmentFileUi, cf, cc, ls);
	ImGui::SameLine();
	ImGui::Text("Parameters");
	drawParameterWidgets(cf.params.insideColoringParameterMaps);
	ImGui::End();
}

void outsideSettingsWindow(clFractal& cf, clCore& cc, clLayerState& ls)
{
	beginFragmentPanel("Outside", "Load Coloring Algorithm",
		cf.outsideColoringCLFragmentFile, cf.outsideColoringCLFragmentFileUi, cf, cc, ls);
	ImGui::SameLine();
	ImGui::Text("Parameters");
	drawParameterWidgets(cf.params.outsideColoringParameterMaps);
	ImGui::Checkbox("Vomit pixel contents", &cf.vomit);
	ImGui::End();
}
