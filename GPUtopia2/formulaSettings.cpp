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
	static std::string outsideColoringName = "by_iteration";
	static std::string insideColoringName = "gaussian_integer";
	if (ImGui::Button("Load Outisde Coloring"))
	{
		bool success = false;
		std::string newOutsideColoringCLFragment;
		openFileDialog(newOutsideColoringCLFragment, success, L"cl", L"GPUtopia CL Fragment (*.cl)");
		if (success)
		{
			cf.outsideColoringCLFragmentFileUi = newOutsideColoringCLFragment;
			std::filesystem::path p(newOutsideColoringCLFragment);
			outsideColoringName = p.stem().string();
		}
	}
	ImGui::Text(outsideColoringName.c_str());
	if (ImGui::Button("Load Coloring"))
	{
		bool success = false;
		std::string newInsideColoringCLFragment;
		openFileDialog(newInsideColoringCLFragment, success, L"cl", L"GPUtopia CL Fragment (*.cl)");
		if (success)
		{
			cf.insideColoringCLFragmentFileUi = newInsideColoringCLFragment;
			std::filesystem::path p(newInsideColoringCLFragment);
			insideColoringName = p.stem().string();
		}
	}
	ImGui::Text(insideColoringName.c_str());
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
		cf.insideColoringCLFragmentFileUi = "clFragments/diag_AA_col.cl";
		cf.outsideColoringCLFragmentFileUi = "clFragments/diag_AA_col.cl";
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
	for (auto& [key, val] : cf.params.fractalParameterMaps.realParameters)
	{
		ImGui::InputDouble(key.c_str(), &(val.first));
	}
	for (auto& [key, val] : cf.params.fractalParameterMaps.complexParameters)
	{
		std::string labelR = key + ".R";
		std::string labelI = key + ".I";
		ImGui::InputDouble(labelR.c_str(), &(val.first.x));
		ImGui::InputDouble(labelI.c_str(), &(val.first.y));
	}
	for (auto& [key, val] : cf.params.fractalParameterMaps.enumParameters)
	{
		std::vector<const char*> enumLabels;
		for (const auto& label : val.first.labels) {
			enumLabels.push_back(label.c_str());
		}
		ImGui::Combo(key.c_str(), &val.first.value, enumLabels.data(), static_cast<int>(enumLabels.size()));
	}
	ImGui::Text("ColoringParameters");
	for (auto& [key, val] : cf.params.insideColoringParameterMaps.integerParameters)
	{
		ImGui::DragInt(key.c_str(), &val.first, 1, 1, 10000);
	}
	for (auto& [key, val] : cf.params.insideColoringParameterMaps.realParameters)
	{
		ImGui::InputDouble(key.c_str(), &val.first);
	}
	for (auto& [key, val] : cf.params.insideColoringParameterMaps.complexParameters)
	{
		std::string labelR = key + ".R";
		std::string labelI = key + ".I";
		ImGui::InputDouble(labelR.c_str(), &(val.first.x));
		ImGui::InputDouble(labelI.c_str(), &(val.first.y));
	}
	for (auto& [key, val] : cf.params.insideColoringParameterMaps.enumParameters)
	{
		std::vector<const char*> enumLabels;
		for (const auto& label : val.first.labels) {
			enumLabels.push_back(label.c_str());
		}
		ImGui::Combo(key.c_str(), &val.first.value, enumLabels.data(), static_cast<int>(enumLabels.size()));
	}
	for (auto& [key, val] : cf.params.outsideColoringParameterMaps.integerParameters)
	{
		ImGui::DragInt(key.c_str(), &val.first, 1, 1, 10000);
	}
	for (auto& [key, val] : cf.params.outsideColoringParameterMaps.realParameters)
	{
		ImGui::InputDouble(key.c_str(), &val.first);
	}
	for (auto& [key, val] : cf.params.outsideColoringParameterMaps.complexParameters)
	{
		std::string labelR = key + ".R";
		std::string labelI = key + ".I";
		ImGui::InputDouble(labelR.c_str(), &(val.first.x));
		ImGui::InputDouble(labelI.c_str(), &(val.first.y));
	}
	for (auto& [key, val] : cf.params.outsideColoringParameterMaps.enumParameters)
	{
		std::vector<const char*> enumLabels;
		for (const auto& label : val.first.labels) {
			enumLabels.push_back(label.c_str());
		}
		ImGui::Combo(key.c_str(), &val.first.value, enumLabels.data(), static_cast<int>(enumLabels.size()));
	}
	ImGui::Checkbox("Vomit pixel contents", &cf.vomit);
	ImGui::End();
}