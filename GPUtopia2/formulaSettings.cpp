#include "formulaSettings.h"



void formulaSettingsWindow(clFractal& cf, clCore& cc)
{
	ImGui::Begin("Formulas");
	static std::string formulaName = "Mandelbrot";
	if (ImGui::Button("Load Formula"))
	{
		bool success = false;
		std::string newFractalCLFragment;
		openFileDialog(newFractalCLFragment, success, L"cl", L"GPUtopia CL Fragment (*.cl)");
		if (success)
		{
			cf.fractalCLFragmentFileUi = newFractalCLFragment;
			std::filesystem::path p(newFractalCLFragment);
			formulaName = p.stem().string();
		}
	}
	ImGui::Text(formulaName.c_str());
	// coloring CL fragment
	static std::string coloringName = "by_iteration";
	if (ImGui::Button("Load Coloring"))
	{
		bool success = false;
		std::string newColoringCLFragment;
		openFileDialog(newColoringCLFragment, success, L"cl", L"GPUtopia CL Fragment (*.cl)");
		if (success)
		{
			cf.coloringCLFragmentFileUi = newColoringCLFragment;
			std::filesystem::path p(newColoringCLFragment);
			coloringName = p.stem().string();
		}
	}
	ImGui::Text(coloringName.c_str());
	if (ImGui::Button("Reload CL Fragments"))
	{
		cf.makeCLCode(SAME_FILES);
		cf.buildKernel = true;
		cc.resetCore();
	}
	ImGui::SameLine();
	if (ImGui::Button("Load AA diag"))
	{
		cf.fractalCLFragmentFileUi = "clFragments/diag_AA.cl";
		cf.coloringCLFragmentFileUi = "clFragments/diag_AA_col.cl";
	}
	static bool useDouble = cf.useDouble;
	ImGui::Checkbox("Double Precision", &useDouble);
	if (!useDouble == cf.useDouble)
	{
		cf.useDouble = useDouble;
		cf.makeCLCode(SAME_FILES);
		cf.buildKernel = true;
		cc.resetCore();
	}
	ImGui::DragFloat("Bailout", &cf.bailout, 4., 20000.);
	ImGui::Text("Formula Parameters:");
	for (auto& [key, val] : cf.params.fractalParameterMaps.integerParameters)
	{
		ImGui::DragInt(key.c_str(), &(val.first), 1, 1, 10000);
	}
	for (auto& [key, val] : cf.params.fractalParameterMaps.floatParameters)
	{
		ImGui::DragFloat(key.c_str(), &(val.first), 0.01f, -100.f, 100.f);
	}
	ImGui::Text("ColoringParameters");
	for (auto& [key, val] : cf.params.coloringParameterMaps.integerParameters)
	{
		ImGui::DragInt(key.c_str(), &val.first, 1, 1, 10000);
	}
	for (auto& [key, val] : cf.params.coloringParameterMaps.floatParameters)
	{
		ImGui::DragFloat(key.c_str(), &val.first, 0.01f, -100.f, 100.f);
	}

	ImGui::Checkbox("Vomit pixel contents", &cf.vomit);
	ImGui::End();
}